#include <graphics.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <commdlg.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

// --- CONFIGURATION ---
#define WIDTH 1366
#define HEIGHT 768
#define KEY_ESC 27
#define KEY_ENTER 13
#define KEY_BACKSPACE 8

// --- COLORS ---
#define COL_BG          COLOR(5, 25, 35)
#define COL_HEADER      COLOR(0, 100, 148)
#define COL_PANEL       COLOR(2, 38, 59)
#define COL_BTN         COLOR(70, 70, 80)
#define COL_BTN_HOVER   COLOR(5, 130, 202)
#define COL_TEXT        WHITE
#define COL_ACCENT      COLOR(177, 215, 230)

// --- STRUCTURE ---
struct Prisoner {
    int id;
    char name[50];
    int age;
    char crime[50];
    double bounty;
    int level;
    int sentence;
    char cell[20];
    char imageFile[260];
    char date[20];
};

// --- GLOBALS ---
int page = 0;

// --- PROTOTYPES ---
void swapBuffers();
void resetMouse();
void drawLayout(const char *title);
int drawButton(int x, int y, int w, int h, const char *text);
void openFileBrowser(char *buffer);
void smartInput(const char *prompt, char *resultDest, int maxLen, int isNum);
void getSystemDate(char *buffer);
int isIdUnique(int id);

// Screens
void runMenu();
void runIntake();
void runSearch();
void runViewAll();
void runRelease();

// Logic
int determineLevel(double bounty);
void generateCell(struct Prisoner *p);
void drawPrisonerCard(struct Prisoner p, int x, int y);

int main() {
    initwindow(WIDTH, HEIGHT, (char*)"IMPEL DOWN SYSTEM - v10 (Color Fix)");
    runMenu();
    closegraph();
    return 0;
}

// --- SYSTEM FUNCTIONS ---

void swapBuffers() {
    setvisualpage(page);
    page = 1 - page;
    setactivepage(page);
    setfillstyle(SOLID_FILL, COL_BG);
    bar(0, 0, WIDTH, HEIGHT);
}

void resetMouse() {
    clearmouseclick(WM_LBUTTONDOWN);
    clearmouseclick(WM_LBUTTONUP);
}

void getSystemDate(char *buffer) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(buffer, "%02d/%02d/%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}

int isIdUnique(int id) {
    FILE *fp = fopen("impel_down.dat", "rb");
    if (!fp) return 1; 
    
    struct Prisoner p;
    while(fread(&p, sizeof(p), 1, fp)) {
        if (p.id == id) {
            fclose(fp);
            return 0; // Duplicate
        }
    }
    fclose(fp);
    return 1; // Unique
}

void openFileBrowser(char *buffer) {
    OPENFILENAME ofn;
    char szFile[260];
    HWND hwnd = GetActiveWindow(); 

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Images\0*.jpg;*.jpeg;*.png;*.bmp\0All\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE) {
        strcpy(buffer, ofn.lpstrFile); 
    } else {
        strcpy(buffer, "No Image Selected");
    }
}

void smartInput(const char *prompt, char *resultDest, int maxLen, int isNum) {
    char tempBuf[100] = "";
    resetMouse();
    while(kbhit()) getch(); 

    while(1) {
        drawLayout("DATA ENTRY MODE");
        
        // Input Area
        setfillstyle(SOLID_FILL, COL_PANEL);
        bar(WIDTH/2 - 250, 300, WIDTH/2 + 250, 500);
        setcolor(WHITE);
        rectangle(WIDTH/2 - 250, 300, WIDTH/2 + 250, 500);
        
        // Text
        setbkcolor(COL_PANEL);
        setcolor(COL_ACCENT);
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 3);
        outtextxy(WIDTH/2 - 230, 330, (char*)prompt);
        
        // Box
        setfillstyle(SOLID_FILL, BLACK);
        bar(WIDTH/2 - 230, 400, WIDTH/2 + 230, 450);
        setcolor(WHITE);
        rectangle(WIDTH/2 - 230, 400, WIDTH/2 + 230, 450);
        
        // Typed Value
        setbkcolor(BLACK);
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 2);
        outtextxy(WIDTH/2 - 220, 415, tempBuf);
        
        setbkcolor(COL_PANEL);
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 1);
        outtextxy(WIDTH/2 - 150, 470, (char*)"ENTER to Next | ESC to Cancel");

        swapBuffers();

        if (kbhit()) {
            char ch = getch();
            if (ch == KEY_ESC) { strcpy(resultDest, "CANCEL"); return; }
            if (ch == KEY_ENTER) {
                if (strlen(tempBuf) > 0) { strcpy(resultDest, tempBuf); return; }
            }
            else if (ch == KEY_BACKSPACE) {
                int len = strlen(tempBuf);
                if (len > 0) tempBuf[len-1] = '\0';
            }
            else if (ch >= 32 && ch <= 126) {
                if (!isNum || (ch >= '0' && ch <= '9') || ch == '.') {
                    int len = strlen(tempBuf);
                    if (len < maxLen) { tempBuf[len] = ch; tempBuf[len+1] = '\0'; }
                }
            }
        }
        delay(30);
    }
}

// --- UI COMPONENTS ---
void drawLayout(const char *title) {
    // Header
    setfillstyle(SOLID_FILL, COL_HEADER);
    bar(0, 0, WIDTH, 100);
    
    setbkcolor(COL_HEADER);
    setcolor(WHITE);
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 5);
    outtextxy(40, 25, (char*)"IMPEL DOWN");
    
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 3);
    outtextxy(WIDTH - 500, 35, (char*)title);

    // FIXED: Footer Moved Up (Height - 80) to avoid Taskbar
    setfillstyle(SOLID_FILL, BLACK);
    bar(0, HEIGHT-80, WIDTH, HEIGHT);
    
    setbkcolor(BLACK);
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 1);
    
    // Developer Name Centered in raised footer
    outtextxy(WIDTH/2 - 200, HEIGHT-50, (char*)"DEV: BASIT ALI SHAH & ABDUL LATIF");
}

int drawButton(int x, int y, int w, int h, const char *text) {
    int mx = mousex();
    int my = mousey();
    int hover = (mx > x && mx < x + w && my > y && my < y + h);
    
    setfillstyle(SOLID_FILL, BLACK);
    bar(x+5, y+5, x+w+5, y+h+5);

    if (hover) setfillstyle(SOLID_FILL, COL_BTN_HOVER);
    else setfillstyle(SOLID_FILL, COL_BTN);
    
    bar(x, y, x+w, y+h);
    setcolor(WHITE);
    rectangle(x, y, x+w, y+h);

    setbkcolor(hover ? COL_BTN_HOVER : COL_BTN);
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 2);
    int tw = textwidth((char*)text);
    int th = textheight((char*)text);
    outtextxy(x + (w-tw)/2, y + (h-th)/2, (char*)text);

    if (hover && ismouseclick(WM_LBUTTONDOWN)) {
        clearmouseclick(WM_LBUTTONDOWN);
        return 1; 
    }
    return 0;
}

// --- SCREENS ---

void runMenu() {
    resetMouse();

    while (1) {
        drawLayout("MAIN DASHBOARD");
        
        int bx = (WIDTH - 400) / 2;
        int startY = 250;
        int gap = 80;

        if (drawButton(bx, startY, 400, 60, "1. INTAKE PRISONER")) { runIntake(); resetMouse(); }
        else if (drawButton(bx, startY + gap, 400, 60, "2. LOCATE PRISONER")) { runSearch(); resetMouse(); }
        else if (drawButton(bx, startY + 2*gap, 400, 60, "3. VIEW ALL RECORDS")) { runViewAll(); resetMouse(); }
        else if (drawButton(bx, startY + 3*gap, 400, 60, "4. RELEASE PRISONER")) { runRelease(); resetMouse(); }
        // Exit Button also raised slightly if needed, but usually okay here
        else if (drawButton(bx, startY + 4*gap, 400, 60, "5. EXIT SYSTEM")) exit(0);

        if (kbhit()) {
            if (getch() == KEY_ESC) exit(0);
        }

        swapBuffers();
        delay(30);
    }
}

void runIntake() {
    struct Prisoner p;
    memset(&p, 0, sizeof(p)); 
    char buffer[100];
    int mode = 0;

    // STEP 0: SELECT MODE
    while(1) {
        drawLayout("INTAKE MODE SELECTION");
        
        setbkcolor(COL_BG);
        setcolor(COL_ACCENT);
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 3);
        outtextxy((WIDTH-400)/2, 250, (char*)"Choose Assignment Method:");

        if (drawButton((WIDTH-400)/2, 350, 400, 60, "1. AUTO ASSIGN LEVEL")) { mode = 1; break; }
        if (drawButton((WIDTH-400)/2, 450, 400, 60, "2. MANUAL LEVEL ENTRY")) { mode = 2; break; }
        
        // FIXED: Back button raised to HEIGHT-150 to avoid footer overlap
        if (drawButton(WIDTH-250, HEIGHT-150, 200, 50, "BACK")) return;

        swapBuffers();
        delay(30);
    }
    resetMouse();

    // STEP 1: ID
    while(1) {
        smartInput("Step 1/6: Enter Prisoner ID", buffer, 10, 1);
        if(strcmp(buffer, "CANCEL") == 0) return;
        
        int tempID = atoi(buffer);
        if (isIdUnique(tempID)) {
            p.id = tempID;
            break; 
        } else {
            setactivepage(page);
            setcolor(RED);
            settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 3);
            outtextxy((WIDTH-500)/2, 600, (char*)"ERROR: ID ALREADY EXISTS!");
            setvisualpage(page);
            delay(2000); 
        }
    }

    smartInput("Step 2/6: Enter Full Name", p.name, 40, 0);
    if(strcmp(p.name, "CANCEL") == 0) return;

    smartInput("Step 3/6: Enter Age", buffer, 3, 1);
    if(strcmp(buffer, "CANCEL") == 0) return;
    p.age = atoi(buffer);

    smartInput("Step 4/6: Enter Crime", p.crime, 40, 0);
    if(strcmp(p.crime, "CANCEL") == 0) return;

    smartInput("Step 5/6: Bounty (Full Amount)", buffer, 15, 1);
    if(strcmp(buffer, "CANCEL") == 0) return;
    p.bounty = atof(buffer);

    if (mode == 1) {
        p.level = determineLevel(p.bounty);
    } else {
        smartInput("MANUAL: Enter Level (1-6)", buffer, 1, 1);
        if(strcmp(buffer, "CANCEL") == 0) return;
        p.level = atoi(buffer);
        if(p.level < 1) p.level = 1;
        if(p.level > 6) p.level = 6;
    }

    while(1) {
        drawLayout("Step 6: Select Photo");
        setcolor(WHITE);
        setbkcolor(COL_BG);
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 2);
        outtextxy(WIDTH/2 - 150, 300, (char*)"Please select a profile picture:");
        
        if (drawButton(WIDTH/2 - 100, 350, 200, 50, "BROWSE PC")) {
            openFileBrowser(p.imageFile);
            break; 
        }
        swapBuffers();
        delay(30);
    }

    while(1) {
        drawLayout("REVIEW & SAVE");
        
        if (strlen(p.date) == 0) getSystemDate(p.date);
        p.sentence = 100;
        generateCell(&p);

        drawPrisonerCard(p, (WIDTH-600)/2, 200);

        setbkcolor(COL_BG); setcolor(YELLOW);
        if(mode == 1) outtextxy((WIDTH-600)/2, 520, (char*)"* Level Auto-Calculated based on Bounty");
        else outtextxy((WIDTH-600)/2, 520, (char*)"* Level Manually Overridden");

        if (drawButton((WIDTH-300)/2, 600, 300, 60, "CONFIRM & SAVE")) {
            FILE *fp = fopen("impel_down.dat", "ab");
            if (fp) { fwrite(&p, sizeof(p), 1, fp); fclose(fp); }
            
            setactivepage(page);
            setcolor(GREEN);
            settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 3);
            outtextxy((WIDTH-400)/2, 700, (char*)"RECORD SAVED SUCCESSFULLY!");
            setvisualpage(page);
            delay(1500);
            return;
        }

        // FIXED: Cancel button raised
        if (drawButton(WIDTH-250, HEIGHT-150, 200, 50, "CANCEL")) return;
        swapBuffers();
        delay(30);
    }
}

void runRelease() {
    char buf[50] = "";
    struct Prisoner targetP;
    bool prisonerFound = false;

    smartInput("Enter ID to Release", buf, 10, 1);
    if(strcmp(buf, "CANCEL") == 0) return;
    
    int searchID = atoi(buf);
    FILE *fp = fopen("impel_down.dat", "rb");
    if(fp) {
        while(fread(&targetP, sizeof(targetP), 1, fp)) {
            if(targetP.id == searchID) { prisonerFound = true; break; }
        }
        fclose(fp);
    }

    if (!prisonerFound) {
        setactivepage(page);
        setfillstyle(SOLID_FILL, COL_BG); bar(0,0,WIDTH,HEIGHT);
        drawLayout("ERROR");
        setcolor(RED);
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 4);
        outtextxy((WIDTH-300)/2, HEIGHT/2, (char*)"ID NOT FOUND!");
        setvisualpage(page);
        delay(1500);
        return;
    }

    while(1) {
        drawLayout("CONFIRM RELEASE");
        drawPrisonerCard(targetP, (WIDTH-600)/2, 200);
        
        setcolor(RED);
        outtextxy((WIDTH-500)/2, 600, (char*)"WARNING: THIS ACTION CANNOT BE UNDONE.");

        if (drawButton((WIDTH-300)/2, 650, 300, 60, "CONFIRM DELETE")) {
            FILE *fp = fopen("impel_down.dat", "rb");
            FILE *ft = fopen("temp.dat", "wb");
            struct Prisoner tempP;
            if(fp && ft) {
                while(fread(&tempP, sizeof(tempP), 1, fp)) {
                    if(tempP.id != targetP.id) fwrite(&tempP, sizeof(tempP), 1, ft);
                }
                fclose(fp); fclose(ft);
                remove("impel_down.dat");
                rename("temp.dat", "impel_down.dat");
                
                setactivepage(page);
                setfillstyle(SOLID_FILL, COL_BG); bar(0,0,WIDTH,HEIGHT);
                drawLayout("SUCCESS");
                setcolor(GREEN);
                outtextxy((WIDTH-300)/2, HEIGHT/2, (char*)"PRISONER RELEASED.");
                setvisualpage(page);
                delay(1500);
                return;
            }
        }
        
        // FIXED: Back button raised
        if (drawButton(WIDTH-250, HEIGHT-150, 200, 50, "CANCEL / BACK")) return;
        swapBuffers();
        delay(30);
    }
}

void runSearch() {
    char buf[50];
    struct Prisoner foundP;
    bool found = false;

    smartInput("Enter ID to Search", buf, 10, 1);
    if(strcmp(buf, "CANCEL") == 0) return;

    int id = atoi(buf);
    FILE *fp = fopen("impel_down.dat", "rb");
    if(fp) {
        while(fread(&foundP, sizeof(foundP), 1, fp)) {
            if(foundP.id == id) { found = true; break; }
        }
        fclose(fp);
    }

    while(1) {
        drawLayout("SEARCH RESULT");
        
        if(found) {
            drawPrisonerCard(foundP, (WIDTH-600)/2, 300);
        } else {
            setcolor(RED);
            settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 4);
            outtextxy((WIDTH-300)/2, HEIGHT/2, (char*)"PRISONER NOT FOUND");
        }
        
        // FIXED: Back button raised
        if (drawButton(WIDTH-250, HEIGHT-150, 200, 50, "BACK TO MENU")) return;
        swapBuffers();
        delay(30);
    }
}

void runViewAll() {
    while(1) {
        drawLayout("ALL RECORDS");
        
        FILE *fp = fopen("impel_down.dat", "rb");
        if(fp) {
            struct Prisoner p;
            int y = 180;
            
            setcolor(COL_ACCENT);
            settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 2);
            
            outtextxy(50, 140, (char*)"ID");
            outtextxy(150, 140, (char*)"NAME");
            outtextxy(400, 140, (char*)"CRIME");
            outtextxy(700, 140, (char*)"LEVEL");
            outtextxy(800, 140, (char*)"CELL");
            outtextxy(950, 140, (char*)"BOUNTY");
            
            setcolor(WHITE);
            line(50, 170, WIDTH-50, 170);

            setcolor(WHITE);
            settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 1);
            char numBuf[50];

            while(fread(&p, sizeof(p), 1, fp)) {
                sprintf(numBuf, "%d", p.id); outtextxy(50, y, numBuf);
                outtextxy(150, y, p.name);
                outtextxy(400, y, p.crime);
                sprintf(numBuf, "%d", p.level); outtextxy(700, y, numBuf);
                outtextxy(800, y, p.cell);
                sprintf(numBuf, "%.0f", p.bounty); outtextxy(950, y, numBuf);

                y += 35;
                if(y > HEIGHT-150) break; 
            }
            fclose(fp);
        } else {
            outtextxy(100, 200, (char*)"Database is empty.");
        }

        // FIXED: Back button raised
        if (drawButton(WIDTH-250, HEIGHT-150, 200, 50, "BACK TO MENU")) return;
        if (kbhit() && getch() == KEY_ESC) return;
        
        swapBuffers();
        delay(30);
    }
}

// --- LOGIC HELPERS ---

void drawPrisonerCard(struct Prisoner p, int x, int y) {
    setfillstyle(SOLID_FILL, COL_PANEL);
    bar(x, y, x+600, y+300);
    setcolor(WHITE);
    rectangle(x, y, x+600, y+300);
    
    rectangle(x+400, y+50, x+550, y+200);
    
    if (access(p.imageFile, F_OK) != -1) {
        readimagefile(p.imageFile, x+401, y+51, x+549, y+199);
    } else {
        outtextxy(x+435, y+110, (char*)"NO IMG");
    }
    
    setbkcolor(COL_PANEL);
    setcolor(COL_ACCENT);
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 4);
    outtextxy(x+20, y+20, (char*)"PRISON CARD");
    
    setcolor(WHITE);
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 1);
    char buf[100];
    sprintf(buf, "NAME: %s", p.name); outtextxy(x+20, y+80, buf);
    sprintf(buf, "ID: %d", p.id); outtextxy(x+20, y+110, buf);
    sprintf(buf, "LEVEL: %d", p.level); outtextxy(x+20, y+140, buf);
    
    sprintf(buf, "CRIME: %s", p.crime); outtextxy(x+20, y+170, buf);
    sprintf(buf, "BOUNTY: %.0f", p.bounty); outtextxy(x+20, y+200, buf);
    
    if (strlen(p.cell) > 0) sprintf(buf, "CELL: %s", p.cell);
    else sprintf(buf, "CELL: N/A");
    outtextxy(x+20, y+230, buf);

    sprintf(buf, "DATE: %s", p.date); outtextxy(x+20, y+260, buf);
}

int determineLevel(double bounty) {
    if (bounty >= 300000000) return 6; 
    if (bounty >= 100000000) return 5; 
    if (bounty >= 50000000)  return 4; 
    if (bounty >= 30000000)  return 3; 
    if (bounty >= 10000000)  return 2; 
    return 1;                          
}

void generateCell(struct Prisoner *p) {
    if (p->level > 0 && p->id > 0) {
        sprintf(p->cell, "L%d-%03d", p->level, p->id);
    } else {
        strcpy(p->cell, "PENDING");
    }
}