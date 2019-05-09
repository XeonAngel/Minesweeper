#include<stdio.h>
#include<stdlib.h>				// Header-ul unde se gaseste functia rand() folosita pentru determinarea unui numar random 
#include<time.h>				// Header-ul folosit pentru srand(time(NULL)) care determina aplicarea algoritmului de calculare a numarului "random" folosint ora calculatorului (milisecundele)
#include<conio.h>				// Header-ul pentru functia system("cls") ,folosita pentru curatarea ecranului
#include<windows.h>				// Header-ul folosit pentru a putea accesa functia SetConsoleTextAttribute ( aceasta schimba culoarea textului )



//Declararea variabilelor globale 

int Minesweeper[18][28];		//Aceasta este matricea propriu-zisa pe care se v-a desfasura jocul

int Minesweeperfals[18][28];	// Aceasta este matricea in care se afla valorile din cea propriu-zisa 
								// O v-om folosi pentru a putea recupera poziitiile minelor si valorile din matrice care v-or fi sterse in urma marcarii cu stegulet

int linii, coloane, minemaxim, sfarsit = 0;
char lit, steag;
int cif, lit2;
int codificare = 90;			//V-om folosi aceasta constanta pentru a putea determina celule din matrice care au fost descoperite de jucator
int charmina = 77;				//V-om folosi aceasta constanta pentru a putea determina caracterul minei
int charsteag = 83;				//V-om folosi aceasta constanta pentru a putea determina caracterul pentru un steag cu care putem marca minele
int minesecrete;				//V-om folosi aceasta constanta pentru a retine numarul real de mine care au fost marcate
int primadescoperire = 0;		//V-om folosi aceasta constanta pentru a vedea daca a fost facuta plasarea si numararea minelor
int liniezero, coloanazero;		//V-om folosi aceste constante pentru a retine coordonatele zerourilor



								//Declararea functiilor prototip

void gotoxy(int x, int y);
void getCursorXY(int &x, int&y);
void afisarematrice(int lin, int col);
void descoperirezero(int lin, int col);
void dificultate();
void initializare();
void mesajsfarsit();
void mesajvictorie();
void adugamine();
void mineinjur();
void descoperire();
void start();
void restart();



//Functie prin care putem duce cursorul oriunde pe fereastra comand promptului, afisa de acolo si ascunde cursorul

void gotoxy(int x, int y)
{
	static HANDLE h = NULL;
	if (!h)
		h = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD c = { x, y };
	SetConsoleCursorPosition(h, c);

	HANDLE hOut;
	CONSOLE_CURSOR_INFO ConCurInf;

	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	ConCurInf.dwSize = 10;
	ConCurInf.bVisible = FALSE;

	SetConsoleCursorInfo(hOut, &ConCurInf);
}

//Funcite care citeste coordonatele cursorului si le scrie in variabile

void getCursorXY(int &x, int&y)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
	{
		x = csbi.dwCursorPosition.X;
		y = csbi.dwCursorPosition.Y;
	}
}

//Funcitie prin care putem selecta dificultatea jocului

void dificultate()
{
	int dif;

	printf("Alege dificultatea : \n\n1 -> Incepator ( 9 x 9 )\n2 -> Intermediar ( 16 x 16 )\n3 -> Expert ( 16 x 26 )\n\n");
	scanf_s("%d", &dif);
	printf("\n");

	switch (dif)
	{
	case 3: linii = 16; coloane = 26; minemaxim = minesecrete = 86;			break;
	case 2: linii = 16; coloane = 16; minemaxim = minesecrete = 40;			break;
	case 1: linii = 9;  coloane = 9;  minemaxim = minesecrete = 10;			break;
	default:
		gotoxy(0, 8);
		printf("Dificultatea selectata nu exista!\n\nVa rog sa reintroduceti dificultatea jocului! \n\n");
		dificultate();
		break;
	}
}

//Initializarea matricii cu codificarea alesa ( 90 = Z ) 

void initializare()
{
	int i, j;

	for (i = 0; i < linii; i++)
	{
		for (j = 0; j < coloane; j++)
		{
			Minesweeper[i][j] = Minesweeperfals[i][j] = codificare;
		}
	}
}

//Subprogram folosit pentru simpla afisare a mesajului  de final 

void mesajsfarsit()
{
	int i, j;

	for (i = 0; i<linii; i++)
	{
		for (j = 0; j < coloane; j++)
		{
			if (Minesweeper[i][j] <= 8)
			{
				Minesweeperfals[i][j] = Minesweeper[i][j];
			}
		}
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
	printf("\n\n\tGAME OVER !\n\n\n");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

//Subprogram folosit pentru simpla afisare a mesajului de victorie

void mesajvictorie()
{
	int i, j;

	for (i = 0; i<linii; i++)
	{
		for (j = 0; j < coloane; j++)
		{
			if (Minesweeper[i][j] <= 8)
			{
				Minesweeperfals[i][j] = Minesweeper[i][j];
			}
		}
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
	printf("\n\n\tFelicitari pentru vitoria obtinuta !\n\n\n");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

//Subprogram care afiseaza matricea

void afisarematrice(int lin, int col)
{
	int i, j;

	//Functia se foloseste pentru a curata comand prompt-ul

	system("cls");

	printf("\t\t");
	printf("========== Minesweeper ==========\n\n");

	//Aici v-om afisa cate mine au mai ramas

	if (sfarsit == 0)
	{
		gotoxy(25, 3);
		printf("Mine ramase : %d\n\n", minemaxim);
	}

	//Afisarea matricii

	//Bordare I

	printf("     ");

	for (i = 0; i < col; i++)
	{
		printf("%c ", 'A' + i);
	}	printf("\n");

	printf("    ");

	for (i = 0; i < 2 * col + 1; i++)
	{
		printf("-");
	}	printf("\n");

	//Afisare matrice propriu-zisa

	for (i = 0; i < lin; i++)
	{
		printf("%#2d | ", i + 1);
		for (j = 0; j < col; j++)
		{
			if (sfarsit == 0)
			{
				if (Minesweeper[i][j] >= codificare || Minesweeper[i][j] == charsteag)
				{
					if (Minesweeper[i][j] == charsteag)
					{
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
						printf("%c ", Minesweeper[i][j]);
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					}
					else
					{
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
						printf("+ ", Minesweeper[i][j]);
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					}
				}
				else
				{
					if (Minesweeper[i][j] == 0)
					{
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
						printf("%d ", Minesweeper[i][j]);
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					}
					else if (Minesweeper[i][j] == 1)
					{
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
						printf("%d ", Minesweeper[i][j]);
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					}
					else if (Minesweeper[i][j] == 2)
					{
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
						printf("%d ", Minesweeper[i][j]);
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					}
					else if (Minesweeper[i][j] > 2 && Minesweeper[i][j] <= 8)
					{
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
						printf("%d ", Minesweeper[i][j]);
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					}
					else
					{
						printf("%d ", Minesweeper[i][j]);
					}
				}
			}
			else
			{
				if (Minesweeperfals[i][j] - codificare == charmina)
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
					printf("%c ", charmina);
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
				}
				else if (Minesweeperfals[i][j] == charsteag)
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
					printf("%c ", charsteag);
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
				}
				else if (Minesweeperfals[i][j] >= 0 && Minesweeperfals[i][j] <= 8)
				{
					if (Minesweeper[i][j] == 0)
					{
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
						printf("%d ", Minesweeper[i][j]);
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					}
					else if (Minesweeper[i][j] == 1)
					{
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
						printf("%d ", Minesweeper[i][j]);
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					}
					else if (Minesweeper[i][j] == 2)
					{
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
						printf("%d ", Minesweeper[i][j]);
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					}
					else if (Minesweeper[i][j] > 2 && Minesweeper[i][j] <= 8)
					{
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
						printf("%d ", Minesweeper[i][j]);
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					}
				}
				else
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
					printf("+ ");
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
				}
			}
			if (j == col - 1)
				printf("| %d", i + 1);

		}
		printf("\n");
	}

	//Bordare II

	printf("    ");

	for (i = 0; i < 2 * col + 1; i++)
	{
		printf("-");
	}
	printf("\n");

	printf("     ");

	for (i = 0; i < col; i++)
	{
		printf("%c ", 'A' + i);
	}
	printf("\n\n\n");
}

//Subprogram prin care adugam minele cu ajutorul functiei rand()

void adugamine()
{
	int u, i, j;
	time_t t;

	//Selectam un seed random cu ajutorul header-urilor stdlib.h si time.h

	srand((unsigned)time(&t));

	for (u = 1; u <= minemaxim; u++)
	{
		i = rand() % linii;
		j = rand() % coloane;
		if (i == cif  &&  j == lit2)
		{
			u--;
		}
		else
		{
			if (Minesweeper[i][j] - codificare == charmina)
			{
				u--;
			}
			else
			{
				Minesweeper[i][j] = Minesweeperfals[i][j] = Minesweeper[i][j] + charmina;
			}
		}
	}
}

//Subprogram cu ajutorul caruia v-om detecta numarul bombelor din jurul unui punct

void mineinjur()
{
	int mine, i, j;

	for (i = 0; i < linii; i++)
	{
		for (j = 0; j < coloane; j++)
		{
			mine = 0;
			if (Minesweeper[i][j] != charmina + codificare)
			{
				if (Minesweeper[i - 1][j - 1] == charmina + codificare)
					mine++;
				if (Minesweeper[i - 1][j] == charmina + codificare)
					mine++;
				if (Minesweeper[i - 1][j + 1] == charmina + codificare)
					mine++;
				if (Minesweeper[i][j - 1] == charmina + codificare)
					mine++;
				if (Minesweeper[i][j + 1] == charmina + codificare)
					mine++;
				if (Minesweeper[i + 1][j - 1] == charmina + codificare)
					mine++;
				if (Minesweeper[i + 1][j] == charmina + codificare)
					mine++;
				if (Minesweeper[i + 1][j + 1] == charmina + codificare)
					mine++;

				Minesweeper[i][j] = Minesweeperfals[i][j] = Minesweeper[i][j] + mine;
			}
		}
	}
	primadescoperire = 1;
}

//Subprogram care in momentul in care descoperim un zero , arata si numerele din jurul sau 

void descoperirezero(int lin, int col)
{
	int i, j;

	if (Minesweeper[lin][col] == 0)
	{
		for (i = lin - 1; i <= lin + 1; i++)
		{
			{
				for (j = col - 1; j <= col + 1; j++)
				{
					if (!(i == lin && j == col) && Minesweeper[i][j] >= 'Z')
					{
						Minesweeper[i][j] -= codificare;
						if (Minesweeper[i][j] == 0)
						{
							descoperirezero(i, j);
						}
					}
				}
			}
		}
	}
}

//Subprogram prin care facem posibila descoperirea spatiilor

void descoperire()
{
	int i, j, x = 0, y = 0;

	if (minesecrete > 0)
	{
		if (primadescoperire)
		{
			getCursorXY(x, y);
			printf("Doriti sa marcati o posibila mina ?\n0 -> Nu\n1 -> Da\n\n");
			scanf_s(" %c", &steag);
			while (!(steag == '0' || steag == '1'))
			{
				gotoxy(x, y - 1);
				printf("Ati introdus gresit !\nDoriti sa marcati o posibila mina ?\n0 -> Nu\n1 -> Da\n\n");
				scanf_s(" %c", &steag);
			}
		}

		printf("Selectati o coloana : ");
		scanf_s(" %c", &lit);
		printf("\n");
		if (int(lit) >= 'a')
			lit2 = (int)lit - 97;
		else
			lit2 = (int)lit - 65;

		printf("Selectati linia : ");
		scanf_s("%d", &cif);
		printf("\n");
		printf("\n\n");

		cif = cif - 1;

		if (steag + 34 == charsteag && !(Minesweeper[cif][lit2] >= 0 && Minesweeper[cif][lit2] <= 8))
		{
			if (Minesweeper[cif][lit2] != charsteag)
			{
				Minesweeper[cif][lit2] = charsteag;
				minemaxim--;
				if (Minesweeperfals[cif][lit2] - codificare == charmina)
				{
					minesecrete--;
				}
			}
			else
			{
				Minesweeper[cif][lit2] = Minesweeperfals[cif][lit2];
				minemaxim++;
				if (Minesweeperfals[cif][lit2] - codificare == charmina)
				{
					minesecrete++;
				}
			}
		}
		else
		{
			if (Minesweeper[cif][lit2] == charsteag)
			{
				afisarematrice(linii, coloane);
				printf("Introduceti alte coordonate\n");
				descoperire();
			}
			else if (Minesweeper[cif][lit2] - codificare == charmina)
			{
				for (i = 0; i<linii; i++)
					for (j = 0; j<coloane; j++)
					{
						Minesweeperfals[i][j] = Minesweeper[i][j];
					}
				sfarsit = -1;
			}
			else
			{
				if (Minesweeper[cif][lit2] >= codificare)
				{
					Minesweeper[cif][lit2] -= codificare;
					if (Minesweeper[cif][lit2] == 0)
					{
						liniezero = cif;
						coloanazero = lit2;
					}
				}
				else
				{
					afisarematrice(linii, coloane);
					printf("Introduceti alte coordonate\n");
					descoperire();
				}
			}
		}

	}
	else
	{
		for (i = 0; i<linii; i++)
			for (j = 0; j<coloane; j++)
			{
				Minesweeperfals[i][j] = Minesweeper[i][j];
			}
		sfarsit = 1;
	}
}

//Subfunctie care porneste si asigura continuitate jocului

void start()
{
	printf("\t\t");
	printf("========== Minesweeper ==========");
	printf("\n\n");

	dificultate();

	initializare();

	afisarematrice(linii, coloane);

	descoperire();

	adugamine();

	mineinjur();

	descoperirezero(liniezero, coloanazero);

	afisarematrice(linii, coloane);

	while (sfarsit == 0)
	{
		descoperire();

		descoperirezero(liniezero, coloanazero);

		afisarematrice(linii, coloane);
	}
	if (sfarsit == 1)
	{
		mesajvictorie();
	}
	else
	{
		mesajsfarsit();
	}
}

//Subprogram prin care putem reporni alt joc

void restart()
{
	int reincepe;
	sfarsit = 0;
	primadescoperire = 0;
	printf("Reincepe un joc \n0->Nu\n1->Da\n\n");
	scanf_s("%d", &reincepe);
	if (reincepe == 1)
	{
		system("cls");
		start();
	}
	else
		exit(0);
}



//functia principala main()

void main()
{
	start();
	restart();
}