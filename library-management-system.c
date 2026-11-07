#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_STR  100
#define MAX_LINE 1000

char logged_in_user[MAX_STR] = "";

void menu_ketabdar(void);
void menu_daneshjoo(void);
void afzoodan_ketabdar(void);
int  vorood_ketabdar(void);
void afzoodan_daneshjoo(void);
int  vorood_daneshjoo(void);
void afzoodan_ketab(void);
void hazf_ketab(void);
void taghir_ettelaat_ketab(void);
void darkhast_ketab(void);
void list_ketabha(void);
void darkhast_ketabdar(void);
void pasdadan_ketab(void);
void sabeghe_ketab(void);

int  get_line(char *str, int size);
void read_input(char *str, int size);
void read_credential(char *str, int size);
int  parse_int(const char *str, int *result);
int  read_int(void);

int  get_next_book_id(void);
void check_and_bootstrap_librarian(void);
void get_timestamp(char *buf, int size);
int  has_pending_request(int bookId, const char *username);

int get_line(char *str, int size) {
    if (fgets(str, size, stdin) == NULL) return 0;
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    } else {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
    return 1;
}

void read_input(char *str, int size) {
    while (1) {
        if (!get_line(str, size)) { str[0] = '\0'; return; }
        if (str[0] == '\0') {
            printf("Meghdar nemitavanad khali bashad. Dobare vared konid: ");
            continue;
        }
        for (int i = 0; str[i] != '\0'; i++) {
            if (str[i] == '/') str[i] = '-';
        }
        return;
    }
}

void read_credential(char *str, int size) {
    while (1) {
        if (!get_line(str, size)) { str[0] = '\0'; return; }
        if (str[0] == '\0') {
            printf("Meghdar nemitavanad khali bashad. Dobare vared konid: ");
            continue;
        }
        if (strchr(str, '/') != NULL) {
            printf("Karaktr '/' dar in field mojaz nist. Dobare vared konid: ");
            continue;
        }
        return;
    }
}

int parse_int(const char *str, int *result) {
    if (str == NULL || *str == '\0') return 0;
    char *end;
    long val = strtol(str, &end, 10);
    if (*end != '\0') return 0;
    if (val < 0 || val > 99999999) return 0;
    *result = (int)val;
    return 1;
}

int read_int(void) {
    char buf[20];
    int val;
    while (1) {
        if (!get_line(buf, sizeof(buf))) return -1;
        if (parse_int(buf, &val)) return val;
        printf("Voroodi namotabar. Adad sahih vared konid: ");
    }
}

void get_timestamp(char *buf, int size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (t == NULL) { buf[0] = '\0'; return; }
    strftime(buf, size, "%Y-%m-%d %H:%M:%S", t);
}

int has_pending_request(int bookId, const char *username) {
    FILE *f = fopen("darkhastha.txt", "r");
    if (f == NULL) return 0;
    char line[MAX_LINE];
    int id;
    char user[MAX_STR], title[MAX_STR];
    int exists = 0;
    while (fgets(line, sizeof(line), f) != NULL) {
        if (sscanf(line, "%d/%99[^/]/%99[^\n]", &id, user, title) == 3) {
            if (id == bookId && strcmp(user, username) == 0) {
                exists = 1;
                break;
            }
        }
    }
    fclose(f);
    return exists;
}

int get_next_book_id(void) {
    FILE *file = fopen("listketab.txt", "r");
    if (file == NULL) return 1001;
    char line[MAX_LINE];
    int max_id = 1000, current_id;
    while (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "%d/", &current_id) == 1) {
            if (current_id > max_id) max_id = current_id;
        }
    }
    fclose(file);
    return max_id + 1;
}

void check_and_bootstrap_librarian(void) {
    FILE *file = fopen("userpassketabdaar.txt", "r");
    int is_empty = 1;
    if (file != NULL) {
        int c = fgetc(file);
        if (c != EOF) is_empty = 0;
        fclose(file);
    }
    if (is_empty) {
        printf("\n--- System Khali Ast: Hich ketabdari yaft nashod. Lotfan avalin ketabdar ra sabt konid ---\n");
        char username[MAX_STR], password[MAX_STR];
        printf("Username Ketabdar Aval: ");
        read_credential(username, sizeof(username));
        printf("Password Ketabdar Aval: ");
        read_credential(password, sizeof(password));

        FILE *wfile = fopen("userpassketabdaar.txt", "w");
        if (wfile == NULL) {
            printf("Khata: nemitavan file userpassketabdaar.txt ra ijad ya baz kard.\n");
            exit(1);
        }
        fprintf(wfile, "%s/%s\n", username, password);
        fclose(wfile);
        printf("Avalin ketabdar ba movafaqiyat sabt shod!\n\n");
    }
}

int main(void) {
    check_and_bootstrap_librarian();

    while (1) {
        logged_in_user[0] = '\0';
        printf("\n=== System Management Ketabkhaneh ===\n");
        printf("1. Vorood Ketabdar\n");
        printf("2. Vorood Daneshjoo\n");
        printf("3. Khorooj az Barnameh\n");
        printf("Entekhab konid: ");

        int intkhab = read_int();
        if (intkhab == -1) break;

        if (intkhab == 1) {
            if (vorood_ketabdar()) menu_ketabdar();
        } else if (intkhab == 2) {
            if (vorood_daneshjoo()) menu_daneshjoo();
        } else if (intkhab == 3) {
            break;
        } else {
            printf("Gozineh namotabar.\n");
        }
    }
    return 0;
}

int vorood_ketabdar(void) {
    char username[MAX_STR], password[MAX_STR];
    char fileLine[MAX_LINE], fUser[MAX_STR], fPass[MAX_STR];

    printf("Username: ");
    read_credential(username, sizeof(username));
    printf("Password: ");
    read_credential(password, sizeof(password));

    FILE *file = fopen("userpassketabdaar.txt", "r");
    if (file == NULL) {
        printf("Khata dar baz kardan file hesab ketabdar.\n");
        return 0;
    }
    while (fgets(fileLine, sizeof(fileLine), file) != NULL) {
        if (sscanf(fileLine, "%99[^/]/%99[^\n]", fUser, fPass) == 2) {
            if (strcmp(username, fUser) == 0 && strcmp(password, fPass) == 0) {
                fclose(file);
                strcpy(logged_in_user, username);
                return 1;
            }
        }
    }
    fclose(file);
    printf("Username ya Password eshtebah ast.\n");
    return 0;
}

int vorood_daneshjoo(void) {
    char username[MAX_STR], password[MAX_STR];
    char fileLine[MAX_LINE], fUser[MAX_STR], fPass[MAX_STR];

    printf("Username: ");
    read_credential(username, sizeof(username));
    printf("Password: ");
    read_credential(password, sizeof(password));

    FILE *file = fopen("userpassdaneshjoo.txt", "r");
    if (file == NULL) {
        printf("Khata dar baz kardan file hesab daneshjoo (Momken ast hich daneshjooyi sabt nashode bashad).\n");
        return 0;
    }
    while (fgets(fileLine, sizeof(fileLine), file) != NULL) {
        if (sscanf(fileLine, "%99[^/]/%99[^\n]", fUser, fPass) == 2) {
            if (strcmp(username, fUser) == 0 && strcmp(password, fPass) == 0) {
                fclose(file);
                strcpy(logged_in_user, username);
                return 1;
            }
        }
    }
    fclose(file);
    printf("Username ya Password eshtebah ast.\n");
    return 0;
}

void menu_ketabdar(void) {
    while (1) {
        printf("\n--- Panel Ketabdar (User: %s) ---\n", logged_in_user);
        printf("1. Sakht Daneshjoo Jadid\n");
        printf("2. Afzoodan Ketab\n");
        printf("3. Hazf Ketab\n");
        printf("4. Taghir Ettelaat Ketab\n");
        printf("5. Didan Darkhasthaye Ketab\n");
        printf("6. Moshahedeh Sabeghe Ketab\n");
        printf("7. Afzoodan Ketabdar Jadid\n");
        printf("8. Khorooj be Menu Asli\n");
        printf("Entekhab konid: ");

        int x = read_int();
        if (x == -1) return;
        if      (x == 1) afzoodan_daneshjoo();
        else if (x == 2) afzoodan_ketab();
        else if (x == 3) hazf_ketab();
        else if (x == 4) taghir_ettelaat_ketab();
        else if (x == 5) darkhast_ketabdar();
        else if (x == 6) sabeghe_ketab();
        else if (x == 7) afzoodan_ketabdar();
        else if (x == 8) break;
        else printf("Gozineh namotabar.\n");
    }
}

void menu_daneshjoo(void) {
    while (1) {
        printf("\n--- Panel Daneshjoo (User: %s) ---\n", logged_in_user);
        printf("1. Darkhast Ketab\n");
        printf("2. Bazgardandan Ketab\n");
        printf("3. Moshahedeh List Ketabha\n");
        printf("4. Khorooj be Menu Asli\n");
        printf("Entekhab konid: ");

        int x = read_int();
        if (x == -1) return;
        if      (x == 1) darkhast_ketab();
        else if (x == 2) pasdadan_ketab();
        else if (x == 3) list_ketabha();
        else if (x == 4) break;
        else printf("Gozineh namotabar.\n");
    }
}

void afzoodan_ketabdar(void) {
    char username[MAX_STR], password[MAX_STR];
    char fileLine[MAX_LINE], fUser[MAX_STR], fPass[MAX_STR];

    printf("Username Jadid: ");
    read_credential(username, sizeof(username));
    printf("Password Jadid: ");
    read_credential(password, sizeof(password));

    FILE *file = fopen("userpassketabdaar.txt", "r");
    if (file != NULL) {
        while (fgets(fileLine, sizeof(fileLine), file) != NULL) {
            if (sscanf(fileLine, "%99[^/]/%99[^\n]", fUser, fPass) == 2) {
                if (strcmp(username, fUser) == 0) {
                    printf("Khata: In Username ghablan sabt shodeh ast!\n");
                    fclose(file);
                    return;
                }
            }
        }
        fclose(file);
    }

    file = fopen("userpassketabdaar.txt", "a");
    if (file == NULL) {
        printf("Khata dar baz kardan file!\n");
        return;
    }
    fprintf(file, "%s/%s\n", username, password);
    fclose(file);
    printf("Ketabdar jadid ba movafaqiyat ezafeh shod.\n");
}

void afzoodan_daneshjoo(void) {
    char username[MAX_STR], password[MAX_STR];
    char fileLine[MAX_LINE], fUser[MAX_STR], fPass[MAX_STR];

    printf("Username Jadid Daneshjoo: ");
    read_credential(username, sizeof(username));
    printf("Password Jadid Daneshjoo: ");
    read_credential(password, sizeof(password));

    FILE *file = fopen("userpassdaneshjoo.txt", "r");
    if (file != NULL) {
        while (fgets(fileLine, sizeof(fileLine), file) != NULL) {
            if (sscanf(fileLine, "%99[^/]/%99[^\n]", fUser, fPass) == 2) {
                if (strcmp(username, fUser) == 0) {
                    printf("Khata: In Username ghablan sabt shodeh ast!\n");
                    fclose(file);
                    return;
                }
            }
        }
        fclose(file);
    }

    file = fopen("userpassdaneshjoo.txt", "a");
    if (file == NULL) {
        printf("Khata dar baz kardan file!\n");
        return;
    }
    fprintf(file, "%s/%s\n", username, password);
    fclose(file);
    printf("Daneshjoo jadid ba movafaqiyat ezafeh shod.\n");
}

void afzoodan_ketab(void) {
    char namketab[MAX_STR], namenevisande[MAX_STR], reshtetahsili[MAX_STR];
    int id = get_next_book_id();

    printf("Nam Ketab: ");
    read_input(namketab, sizeof(namketab));
    printf("Nam Nevisande: ");
    read_input(namenevisande, sizeof(namenevisande));
    printf("Reshte Tahsili ya Omomi: ");
    read_input(reshtetahsili, sizeof(reshtetahsili));

    FILE *file = fopen("listketab.txt", "a");
    if (file == NULL) {
        printf("Khata dar baz kardan file!\n");
        return;
    }
    fprintf(file, "%d/%s/%s/%s/khali/none\n", id, namketab, namenevisande, reshtetahsili);
    fclose(file);
    printf("Ketab jadid ba ID [%d] ba movafaqiyat ezafeh shod.\n", id);
}

void hazf_ketab(void) {
    int fileId;
    char fileTitle[MAX_STR], fileAuthor[MAX_STR], reshtehtahsili[MAX_STR], vaziyat[MAX_STR], borrower[MAX_STR];
    char line[MAX_LINE];

    printf("ID ketabi ke mikhahid hazf konid: ");
    int target_id = read_int();
    if (target_id < 0) return;

    FILE *ptrfile = fopen("listketab.txt", "r");
    if (ptrfile == NULL) {
        printf("File ketabha yaft nashod.\n");
        return;
    }
    FILE *temp = fopen("temp.txt", "w");
    if (temp == NULL) { fclose(ptrfile); return; }

    int found = 0, borrowed_error = 0;
    while (fgets(line, sizeof(line), ptrfile) != NULL) {
        if (sscanf(line, "%d/%99[^/]/%99[^/]/%99[^/]/%99[^/]/%99[^\n]",
                   &fileId, fileTitle, fileAuthor, reshtehtahsili, vaziyat, borrower) == 6) {
            if (fileId == target_id) {
                if (strcmp(vaziyat, "por") == 0) {
                    borrowed_error = 1;
                    fprintf(temp, "%s", line);
                } else {
                    found = 1;
                }
            } else {
                fprintf(temp, "%s", line);
            }
        } else {
            fprintf(temp, "%s", line);
        }
    }
    fclose(ptrfile);
    fclose(temp);

    if (borrowed_error) {
        remove("temp.txt");
        printf("Khata: In ketab dar hal hazer amanat dadeh shodeh ast va nemitavanid an ra hazf konid!\n");
    } else if (found) {
        remove("listketab.txt");
        rename("temp.txt", "listketab.txt");
        printf("Ketab ba ID [%d] ba movafaqiyat hazf shod.\n", target_id);
    } else {
        remove("temp.txt");
        printf("Ketab ba in ID yaft nashod.\n");
    }
}

void taghir_ettelaat_ketab(void) {
    int fileId;
    char fileTitle[MAX_STR], fileAuthor[MAX_STR], reshtehtahsili[MAX_STR], vaziyat[MAX_STR], borrower[MAX_STR];
    char newTitle[MAX_STR], newAuthor[MAX_STR], newReshte[MAX_STR];
    char line[MAX_LINE];

    printf("ID ketabi ke mikhahid ettelaat an ra taghir dahid: ");
    int target_id = read_int();
    if (target_id < 0) return;

    FILE *ptrfile = fopen("listketab.txt", "r");
    if (ptrfile == NULL) {
        printf("File ketabha yaft nashod.\n");
        return;
    }
    FILE *temp = fopen("temp.txt", "w");
    if (temp == NULL) { fclose(ptrfile); return; }

    int found = 0;
    while (fgets(line, sizeof(line), ptrfile) != NULL) {
        if (sscanf(line, "%d/%99[^/]/%99[^/]/%99[^/]/%99[^/]/%99[^\n]",
                   &fileId, fileTitle, fileAuthor, reshtehtahsili, vaziyat, borrower) == 6) {
            if (fileId == target_id && !found) {
                found = 1;
                printf("Nam Jadid Ketab: ");
                read_input(newTitle, sizeof(newTitle));
                printf("Nam Jadid Nevisande: ");
                read_input(newAuthor, sizeof(newAuthor));
                printf("Reshte Tahsili Jadid: ");
                read_input(newReshte, sizeof(newReshte));
                fprintf(temp, "%d/%s/%s/%s/%s/%s\n",
                        fileId, newTitle, newAuthor, newReshte, vaziyat, borrower);
            } else {
                fprintf(temp, "%s", line);
            }
        } else {
            fprintf(temp, "%s", line);
        }
    }
    fclose(ptrfile);
    fclose(temp);

    if (found) {
        remove("listketab.txt");
        rename("temp.txt", "listketab.txt");
        printf("Ettelaat ketab ba movafaqiyat virayesh shod.\n");
    } else {
        remove("temp.txt");
        printf("Ketab yaft nashod.\n");
    }
}

void darkhast_ketab(void) {
    int fileId;
    char fileline[MAX_LINE], filenameketab[MAX_STR], filenevisande[MAX_STR], fileReshte[MAX_STR], vaziyat[MAX_STR], borrower[MAX_STR];
    int available = 0;

    printf("ID Ketab Darkhasti: ");
    int target_id = read_int();
    if (target_id < 0) return;

    if (has_pending_request(target_id, logged_in_user)) {
        printf("Shoma ghablan baraye in ketab darkhast sabt kardeh id.\n");
        return;
    }

    FILE *file = fopen("listketab.txt", "r");
    if (file == NULL) {
        printf("List ketabha yaft nashod!\n");
        return;
    }
    while (fgets(fileline, sizeof(fileline), file) != NULL) {
        if (sscanf(fileline, "%d/%99[^/]/%99[^/]/%99[^/]/%99[^/]/%99[^\n]",
                   &fileId, filenameketab, filenevisande, fileReshte, vaziyat, borrower) == 6) {
            if (fileId == target_id) {
                if (strcmp(vaziyat, "khali") == 0) {
                    available = 1;
                } else {
                    printf("Ketab ham aknoon dar amanat (%s) ast.\n", borrower);
                    fclose(file);
                    return;
                }
                break;
            }
        }
    }
    fclose(file);

    if (available) {
        FILE *req = fopen("darkhastha.txt", "a");
        if (req != NULL) {
            fprintf(req, "%d/%s/%s\n", target_id, logged_in_user, filenameketab);
            fclose(req);
            printf("Darkhast ketab ba ID [%d] tavasot '%s' sabt shod.\n", target_id, logged_in_user);
        } else {
            printf("Khata dar sabt darkhast.\n");
        }
    } else {
        printf("Ketab ba in ID yaft nashod.\n");
    }
}

void list_ketabha(void) {
    int fileId;
    char fileTitle[MAX_STR], fileAuthor[MAX_STR], reshtehtahsili[MAX_STR], vaziyat[MAX_STR], borrower[MAX_STR];
    char line[MAX_LINE];

    FILE *file = fopen("listketab.txt", "r");
    if (file == NULL) {
        printf("Ketabi peyda nashod!\n");
        return;
    }
    printf("\n--- List Ketabha ---\n");
    int count = 0;
    while (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "%d/%99[^/]/%99[^/]/%99[^/]/%99[^/]/%99[^\n]",
                   &fileId, fileTitle, fileAuthor, reshtehtahsili, vaziyat, borrower) == 6) {
            printf("ID: %d | Nam Ketab: %s | Nevisande: %s | Reshte: %s\n",
                   fileId, fileTitle, fileAuthor, reshtehtahsili);
            if (strcmp(vaziyat, "khali") == 0) {
                printf("Vaziyat: Azad\n");
            } else {
                printf("Vaziyat: Amanat dadeh shodeh be (%s)\n", borrower);
            }
            printf("-------------------\n");
            count++;
        }
    }
    if (count == 0) printf("Ketabi dar system sabt nashodeh ast.\n");
    fclose(file);
}

void darkhast_ketabdar(void) {
    int reqId;
    char username[MAX_STR], bookTitle[MAX_STR];
    char fileline[MAX_LINE], decision_buf[16];
    char bTitle[MAX_STR], bAuthor[MAX_STR], bReshte[MAX_STR], bVaziyat[MAX_STR], bBorrower[MAX_STR];
    int bId;
    char ts[50];

    FILE *reqFile = fopen("darkhastha.txt", "r");
    if (reqFile == NULL) {
        printf("Darkhasti voojood nadarad.\n");
        return;
    }
    FILE *tempReqFile = fopen("temp_darkhastha.txt", "w");
    FILE *resFile     = fopen("natijeh.txt", "a");
    if (tempReqFile == NULL || resFile == NULL) {
        fclose(reqFile);
        if (tempReqFile) fclose(tempReqFile);
        if (resFile)     fclose(resFile);
        return;
    }

    int totalRequests = 0;
    while (fgets(fileline, sizeof(fileline), reqFile) != NULL) {
        totalRequests++;
        if (sscanf(fileline, "%d/%99[^/]/%99[^\n]", &reqId, username, bookTitle) != 3) {
            fprintf(tempReqFile, "%s", fileline);
            continue;
        }

        FILE *bookFile     = fopen("listketab.txt", "r");
        FILE *tempBookFile = fopen("temp_listketab.txt", "w");
        if (bookFile == NULL || tempBookFile == NULL) {
            if (bookFile)     fclose(bookFile);
            if (tempBookFile) fclose(tempBookFile);
            remove("temp_listketab.txt");
            fprintf(tempReqFile, "%s", fileline);
            continue;
        }

        int foundBook = 0, processed = 0;
        char bLine[MAX_LINE];
        while (fgets(bLine, sizeof(bLine), bookFile) != NULL) {
            if (sscanf(bLine, "%d/%99[^/]/%99[^/]/%99[^/]/%99[^/]/%99[^\n]",
                       &bId, bTitle, bAuthor, bReshte, bVaziyat, bBorrower) == 6) {
                if (!processed && bId == reqId) {
                    foundBook = 1;
                    printf("\nDarkhast ketab '%s' (ID: %d) tavasot karbar '%s'\n", bTitle, bId, username);
                    printf("Aya mipazirid? (y/n): ");
                    read_input(decision_buf, sizeof(decision_buf));
                    get_timestamp(ts, sizeof(ts));

                    if (decision_buf[0] == 'y' || decision_buf[0] == 'Y') {
                        if (strcmp(bVaziyat, "khali") == 0) {
                            fprintf(tempBookFile, "%d/%s/%s/%s/por/%s\n",
                                    bId, bTitle, bAuthor, bReshte, username);
                            fprintf(resFile, "%d/%s/%s/Accepted/%s\n",
                                    bId, bTitle, username, ts);
                            printf("Pazirofteh shod.\n");
                        } else {
                            fprintf(tempBookFile, "%s", bLine);
                            fprintf(resFile, "%d/%s/%s/Rejected - Already Borrowed/%s\n",
                                    bId, bTitle, username, ts);
                            printf("Ketab ghablan amanat dadeh shodeh ast.\n");
                        }
                    } else {
                        fprintf(tempBookFile, "%s", bLine);
                        fprintf(resFile, "%d/%s/%s/Rejected/%s\n",
                                bId, bTitle, username, ts);
                        printf("Rad shod.\n");
                    }
                    processed = 1;
                } else {
                    fprintf(tempBookFile, "%s", bLine);
                }
            } else {
                fprintf(tempBookFile, "%s", bLine);
            }
        }
        fclose(bookFile);
        fclose(tempBookFile);

        if (foundBook) {
            remove("listketab.txt");
            rename("temp_listketab.txt", "listketab.txt");
        } else {
            remove("temp_listketab.txt");
            get_timestamp(ts, sizeof(ts));
            fprintf(resFile, "%d/%s/%s/Rejected - Book Not Found/%s\n",
                    reqId, bookTitle, username, ts);
            printf("Ketab ba ID [%d] yaft nashod. Darkhast rad shod.\n", reqId);
        }
    }

    fclose(reqFile);
    fclose(tempReqFile);
    fclose(resFile);
    remove("darkhastha.txt");
    rename("temp_darkhastha.txt", "darkhastha.txt");

    if (totalRequests == 0) {
        printf("Hich darkhasti baraye pardazesh voojood nadasht.\n");
    }
}

void pasdadan_ketab(void) {
    int fileId;
    char fileTitle[MAX_STR], fileAuthor[MAX_STR], reshtehtahsili[MAX_STR], vaziyat[MAX_STR], borrower[MAX_STR];
    char line[MAX_LINE];
    char ts[50];

    printf("ID ketabi ke mikhahid pas dahid: ");
    int target_id = read_int();
    if (target_id < 0) return;

    FILE *ptrfile = fopen("listketab.txt", "r");
    if (ptrfile == NULL) {
        printf("File ketabha yaft nashod.\n");
        return;
    }
    FILE *temp = fopen("temp.txt", "w");
    if (temp == NULL) { fclose(ptrfile); return; }

    int found = 0;
    while (fgets(line, sizeof(line), ptrfile) != NULL) {
        if (sscanf(line, "%d/%99[^/]/%99[^/]/%99[^/]/%99[^/]/%99[^\n]",
                   &fileId, fileTitle, fileAuthor, reshtehtahsili, vaziyat, borrower) == 6) {
            if (fileId == target_id &&
                strcmp(vaziyat, "por") == 0 &&
                strcmp(borrower, logged_in_user) == 0 && !found) {
                fprintf(temp, "%d/%s/%s/%s/khali/none\n",
                        fileId, fileTitle, fileAuthor, reshtehtahsili);
                get_timestamp(ts, sizeof(ts));
                FILE *resFile = fopen("natijeh.txt", "a");
                if (resFile != NULL) {
                    fprintf(resFile, "%d/%s/%s/Returned/%s\n",
                            fileId, fileTitle, logged_in_user, ts);
                    fclose(resFile);
                }
                found = 1;
            } else {
                fprintf(temp, "%s", line);
            }
        } else {
            fprintf(temp, "%s", line);
        }
    }
    fclose(ptrfile);
    fclose(temp);

    if (found) {
        remove("listketab.txt");
        rename("temp.txt", "listketab.txt");
        printf("Ketab ba ID [%d] ba movafaqiyat tahvil gerefteh shod.\n", target_id);
    } else {
        remove("temp.txt");
        printf("Ketab yaft nashod ya dar amanat shoma nist.\n");
    }
}

void sabeghe_ketab(void) {
    char fileline[MAX_LINE];
    int fileId;
    char fileTitle[MAX_STR], fileUsername[MAX_STR], filevaziyat[MAX_STR], ts[50];
    int found = 0;

    printf("ID ketab ra baraye didan sabeghe vared konid: ");
    int target_id = read_int();
    if (target_id < 0) return;

    FILE *file = fopen("natijeh.txt", "r");
    if (file == NULL) {
        printf("Sabeghei yaft nashod!\n");
        return;
    }

    printf("\nSabeghe ketab ID [%d]:\n", target_id);
    while (fgets(fileline, sizeof(fileline), file) != NULL) {
        int n = sscanf(fileline, "%d/%99[^/]/%99[^/]/%99[^/]/%49[^\n]",
                       &fileId, fileTitle, fileUsername, filevaziyat, ts);
        if (n == 5) {
        } else if (sscanf(fileline, "%d/%99[^/]/%99[^/]/%99[^\n]",
                          &fileId, fileTitle, fileUsername, filevaziyat) == 4) {
            ts[0] = '\0';
        } else {
            continue;
        }
        if (fileId == target_id) {
            if (ts[0] != '\0') {
                printf("Ketab: %s | Karbar: %s | Vaziyat: %s | Zaman: %s\n",
                       fileTitle, fileUsername, filevaziyat, ts);
            } else {
                printf("Ketab: %s | Karbar: %s | Vaziyat: %s\n",
                       fileTitle, fileUsername, filevaziyat);
            }
            found = 1;
        }
    }
    fclose(file);

    if (!found) {
        printf("Baraye ketab ba ID [%d] sabeghei sabt nashodeh ast.\n", target_id);
    }
}