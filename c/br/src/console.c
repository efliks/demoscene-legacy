

/* ====================================
	coded by Mikolaj Feliks aka Majuma
	mail: majuma@wp.pl
	web: http://www.majuma.xt.pl
	all rights reserved
   ==================================== */



COMMAND CommandList[MAX_COMMANDS] = {{"CMDLIST", 0, cListCommands},
	{"VARLIST", 0, cListVars},
	{"INTROMSG", 0, cIntroMsg},
	{"QUIT", 0, cQuitGame},
	{"CLEAR", 0, cClearConsole},
	{"BUILD", 0, cBuildTime},
	{"MAP", 1, cLoadMap},
	{"MAPINFO", 0, cMapInfo},
	{"FREEMAP", 0, cFreeMap},
	{"CENTER", 0, cCenterMap},
	{"DUMP", 0, cDump},
	{"SET", 2, cSetVar},
	{"GET", 1, cGetVar}};

//------------------------------------------------------------

VAR VarList[MAX_VARS] = {{"V_CONCOL", &ConColor, 0, 255, 0},
	{"V_CONADDCOL", &ConAddCol, 0, 255, 0},
	{"V_GAMECOL", &GameColor, 0, 255, 0},
	{"V_GAMEADDCOL", &GameAddCol, 0, 255, 0},
	{"V_LOGFLAG", &LogFlag, 0, 1, StartStopLog},
	{"V_MAPPOS", &ShowMapPos, 0, 1, 0},
	{"V_SHOWDEAD", &ShowDead, 0, 1, 0},
	{"V_DUMPFORMAT", &DumpFormat, 0, 1},
	{"V_SCROLLSPD", &ScrollSpeed, 1, 5, 0},
	{"V_MOUSESPD", &MouseSpeed, 0, 31, SetMouseSpeed},
	{"V_FADEFLAG", &FadeFlag, 0, 1, 0},
	{"V_FRAMECOL", &FrameCol, 0, 255, 0},
	{"V_SELCOL", &SelCol, 0, 255, 0},
	{"V_SELCOLE", &SelColE, 0, 255, 0},
	{"V_ISCLOCK", &IsClock, 0, 1, 0},
	{"V_MMAPCOL0", &MmapCol0, 0, 255, 0},
	{"V_MMAPCOL1", &MmapCol1, 0, 255, 0},
	{"V_MMAPRDIR", &MmapRDir, -1, 1, 0},
	{"V_MMAPRSPD", &MmapRSpd, 1, 5, 0},
	{"V_MMAPRCOL", &MmapColR, 0, 255, 0}};

//------------------------------------------------------------
//------------------------------------------------------------

char *ConsoleData;
int ConsoleLine, ConsoleChar, ConColor = CON_COLOR,
	ConAddCol = CON_ADDCOL, LogFlag = 0, ConImgFlag = 0;
BYTE *ConsoleImg;

char GlobalArg1[ARG_LEN], GlobalArg2[ARG_LEN],
	GlobalArg3[ARG_LEN];

//------------------------------------------------------------

int SetConsole(void) {
	int r;

	ConsoleData = (char *)malloc(40 * 25);
	if (!ConsoleData) {
		return 0;
	}

	ClearConsole();

	return 1;
}

void FreeConsole(void) {
	free(ConsoleData);
}

int LoadConsoleImg(char *ImgFilename) {
	int r;

	ConsoleImg = (BYTE *)malloc(64000);
	if (!ConsoleImg) {
		return -1;
	}

	r = LoadBMP(ImgFilename, ConsoleImg, 0, 320, 200);
	if (r < 0) {
		free(ConsoleImg);
		return -2;
	}

	ConImgFlag = 1;

	return 1;
}

void FreeConsoleImg(void) {
	free(ConsoleImg);
}

void ClearConsole(void) {
	memset(ConsoleData, 0, 40 * 25);

	ConsoleLine = 0;
	ConsoleChar = 0;
}

void AddConsoleLine(char *line) {
	char *ptr = ConsoleData + (ConsoleLine * 40), c, *s, *d;
	int i;

	i = 0;
	while (((c = line[i++]) != 0) && (i < 40)) {
		*ptr = c;
		ptr++;
	}

	while (i != 40) {
		*ptr = 0;
		i++;
	}

	if ((++ConsoleLine) == 25) {
		ConsoleLine = 24;
		
		d = ConsoleData;
		s = d + 40;

		for (i = 0; i < 40 * 24; i++, s++, d++) *d = *s;

		for (i = 0; i < 40; i++) *d = 0;
	}

	ConsoleChar = 0;
}

void PrintConsole(void) {
	int i;
	char *str = ConsoleData;

	if (ConImgFlag) {
		memcpy(FrameBuffer, ConsoleImg, 64000);
	}
	else {
		ClearFrameBuffer();
	}

	for (i = 0; i < ConsoleLine; i++) {
		PutString(str, 0, i << 3, ConColor, ConAddCol);
		str += 40;
	}
}

void PutCursor(void) {
	char cur[1];

	cur[0] = 32 + 64;
	cur[1] = 0;

	PutString(cur, ConsoleChar << 3, ConsoleLine << 3, ConColor,
		ConAddCol);
}

int Console(void) {
	char c, CurrStr[40];
	int n_chars, r = 1, i, CurTimer = 0, IsCursor, n_loops = 0;
	long addptr;

	ConsoleChar = 0;

	PrintConsole();

	PutCursor();

	n_chars = 0;
	CurrStr[0] = 0;
	
	while (1) {
		if (kbhit()) {
			c = getch();

			if (c == KEY_ENTER) {
				if (n_chars) {										
					r = ParseCommand(CurrStr);
					if (!r) break;
					
					PrintConsole();

					n_chars = 0;
					CurrStr[0] = 0;
				}
			}
			else if (c == KEY_BACKSPACE) {
				if (n_chars > 0) {
					CurrStr[--n_chars] = 0;
					ConsoleChar--;
				}
			}
			else if (c == KEY_ESCAPE) {
				r = 2;
				break;
			}
			else if (c == KEY_CONSOLE) {
				r = 1;
				break;
			}
			else {
				if (n_chars < 38) {
					CurrStr[n_chars++] = toupper(c);
					CurrStr[n_chars] = 0;
					ConsoleChar++;
				}
			}

			addptr = (ConsoleLine << 3) * 320;
			memcpy(FrameBuffer + addptr, ConsoleImg + addptr, 320 * 8);

			PutString(CurrStr, 0, ConsoleLine << 3, ConColor, ConAddCol);
		}

		if ((++CurTimer) == CUR_TIMER) {
			CurTimer = 0;
			IsCursor++;
			IsCursor &= 1;
		}

		if (IsCursor) {
			PutCursor();
		}
		else {
			addptr = (ConsoleLine << 3) * 320 + (ConsoleChar << 3);
			for (i = 0; i < 8; i++) {
				memcpy(FrameBuffer + addptr, ConsoleImg + addptr, 8);
				addptr += 320;
			}
		}


		Wait(2);
		CopyFrameBuffer();

		if (!(n_loops++)) FadeUp();
	}

	FadeDown();

	return r;
}

//------------------------------------------------------------

int ParseCommand(char *line) {
	int i, waschar, k, j, error, found, r = 1, n_args;
	char c, cmd[COMM_LEN], args[3][ARG_LEN];
	COMMAND *command = CommandList;

	Print(line);

	i = waschar = k = j = error = 0;
	cmd[0] = args[0][0] = args[1][0] = args[2][0] = 0;

	while (1) {
		c = line[i++];

		if (c == ' ' || c == 0) {
			if (!waschar) continue;

			if (!k)
				cmd[j] = 0;
			else
				args[k - 1][j] = 0;

			j = 0;

			//if ((++k) == 4) break;
			if ((++k) == 3) break;

			if (!c) break;
		}
		else {
			waschar = 1;

			if (!k) {
				if (j == COMM_LEN) { error = 1; break; }
				cmd[j++] = c;
			}
			else {
				if (j == ARG_LEN) { error = 1; break; }
				args[k - 1][j++] = c;
			}
		}
	}

	if (error) {
		Print("ERROR");
		return -1;
	}

	n_args = 0;
	for (i = 0; i < 3; i++) {
		if (args[i][0] != 0)
			n_args++;
		else
			break;
	}

	found = 0;
	for (i = 0; i < MAX_COMMANDS; i++, command++) {
		if (strcmp(command->Command, cmd) == 0) {
			found = 1;
			if (command->NumArgs < n_args) error = 1;
			break;
		}
	}

	if (!found) {
		Print("UNRECOGNIZED COMMAND");
		return -2;
	}

	if (error) {
		Print("INCORRECT NUMBER OF ARGUMENTS");
		return -3;
	}

	for (i = 0; i < ARG_LEN; i++) {
		GlobalArg1[i] = args[0][i];
		GlobalArg2[i] = args[1][i];
		//GlobalArg3[i] = args[2][i];
	}

	r = command->f();
	
	return r;
}

int LoadCfg(void) {
	FILE *fp;
	int NumLines = 0, LineCnt = 0, ReadMode = 1, Open = 0, i;
	char Lines[MAX_CFGLINES][40], c;

	fp = fopen("autoexec.cfg", "r");
	if (!fp) {
		Print("AUTOEXEC.CFG NOT FOUND");
		return -1;
	}

	while (!feof(fp)) {
		c = fgetc(fp);

		if (c == '{') {
			ReadMode = 0;
			Open++;
			continue;
		}
		
		if (c == '}') {
			if (ReadMode) {
				fclose(fp);
				Print("COMMENT ERROR IN AUTOEXEC.CFG");
				return -2;
			}

			Open--;
			if (!Open) ReadMode = 1;
			continue;
		}

		if (!ReadMode) continue;

		if (c != '\n') {
			if (c >= 32) {
				if (LineCnt == 39) {
					fclose(fp);
					Print("LINE IN AUTOEXEC.CFG IS TOO LONG");
					return -3;
				}
				Lines[NumLines][LineCnt++] = toupper(c);
			}
		}
		else {
			if (LineCnt) {
				Lines[NumLines][LineCnt] = 0;
				LineCnt = 0;
				NumLines++;

				if (NumLines == MAX_CFGLINES) {
					Print("TOO MANY LINES IN AUTOEXEC.CFG");
					return -4;
				}
			}
		}
	}

	fclose(fp);

	if (Open) {
		Print("COMMENT IN AUTOEXEC.CFG IS OPEN");
		return -5;
	}

	if (!NumLines) {
		Print("AUTOEXEC.CFG IS EMPTY");
		return -6;
	}

	for (i = 0; i < NumLines; i++) {
		ParseCommand(Lines[i]);
		Show();
	}

	return NumLines;
}

void StartStopLog(void) {
	if (LogFlag) {
		unlink("console.log");
		Print("LOGGING STARTED");
	}
	else {
		Print("LOGGING STOPPED");
	}
}

void LogLine(char *line) {
	FILE *p;

	p = fopen("console.log", "a");
	if (!p) {

		//Print("UNABLE TO LOG LINE");
		return;
	}

	fprintf(p, "%s\n", line);
	fclose(p);
}

int IsInt(char *str) {
	int i = 0;
	char c;

	if (!str[0]) return 0;

	while (1) {
		c = str[i++];
		if (c == 0) break;
		
		if (((c >= '0') && (c <= '9')) || ((i == 1) && (c == '-')))
			continue;
		else
			return 0;
	}

	return 1;
}

//------------------------------------------------------------
// COMMANDS
// Call this functions only through ParseCommand()
//------------------------------------------------------------

int cListCommands(void) {
	int i;
	COMMAND *c = CommandList;

	for (i = 0; i < MAX_COMMANDS; i++, c++) {
		if (c->Command[0] == '.') continue;
		Print(c->Command);
	}

	return 1;
}

int cListVars(void) {
	int i;
	VAR *v = VarList;
	char str[40];

	strcpy(str, "");

	for (i = 0; i < MAX_VARS; i++, v++) {
		if ((strlen(str) + strlen(v->VarName)) < 39) {
			strcat(str, v->VarName);
			strcat(str, " ");
		}
		else {
			Print(str);
			strcpy(str, v->VarName);
			strcat(str, " ");
		}
	}

	if (strlen(str)) {
		Print(str);
	}

	return 1;
}

int cIntroMsg(void) {
	Print("BLOOD RED V.0.1 BY MAJUMA");
	Print("TYPE 'CMDLIST' FOR COMMAND LIST");
	Print("PRESS TYLDE TO ENTER GAME");
	return 1;
}

int cQuitGame(void) {
	return 0;
}

int cClearConsole(void) {
	ClearConsole();
	return 1;
}

int cBuildTime(void) {
	Print(strupr(BUILD_STR));
	return 1;
}

int cLoadMap(void) {
	char str[64], t;
	int i, r;

	if (strcmp(GlobalArg1, "") == 0) {
		Print("USAGE: MAP FILENAME.MAP");

		if (MapIsLoaded) {
			strcpy(str, CurrentMap.Filename);
			for (i = 0; i < strlen(str); i++) {
				t = str[i];
				str[i] = toupper(t);
			}
			strcat(str, " IS LOADED");
			Print(str);
		}
	}
	else {
		if (MapIsLoaded) {
			FreeMap();
			MapIsLoaded = 0;
			Print("MAP FREED");
		}

		strcpy(str, "LOADING ");
		strcat(str, GlobalArg1);
		strcat(str, "...");
		Print(str);
		Show();

		r = LoadMap(GlobalArg1);

		switch (r) {
			case LM_OK:
				MapIsLoaded = 1;
				Print("MAP LOADED");
				DoClock(1);	// reset clock
				break;
			case LM_NOFILE:
				Print("FILE NOT FOUND");
				break;
			case LM_FORMATERR:
				Print("INCORRECT FILE FORMAT");
				break;
			case LM_VERERR:
				Print("INCORRECT MAP VERSION");
				break;
			case LM_TEXERR:
				Print("UNABLE TO ALLOC TEXTURE DATA");
				break;
			case LM_CELLERR:
				Print("UNABLE TO ALLOC CELL DATA");
				break;
			case LM_SCERR:
				Print("UNABLE TO ALLOC SMALL CELL DATA");
				break;
			case LM_UNITERR:
				Print("UNABLE TO ALLOC UNIT DATA");
				break;
			case LM_BULLERR:
				Print("UNABLE TO ALLOC BULLET DATA");
				break;
			case LM_EXPLERR:
				Print("UNABLE TO ALLOC EXPL DATA");
				break;
			case LM_SPRERR:
				for (i = 0; i < strlen(SprErrMsg); i++) {
					if (i == 39) break;
					str[i] = toupper(SprErrMsg[i]);
				}
				str[i] = 0;
				Print(str);
				break;
			default:
				strcpy(str, TexErrFile);

				switch (r) {
					case LT_MEMERR:
						strcat(str, ": UNABLE TO ALLOC");
						break;
					case LT_NOFILE:
						strcat(str, ": FILE NOT FOUND");
						break;
					case LT_FORMATERR:
						strcat(str, ": FORMAT IS INCORRECT");
						break;
					case LT_SIZEERR:
						strcat(str, ": SIZE IS INCORRECT");
						break;
					default: //case LT_BITSERR:
						strcat(str, ": INCORRECT COLOR DEPTH");
				}

				for (i = 0; i < strlen(str); i++) {
					if (i == 39) break;
					str[i] = toupper(str[i]);
				}
				str[i] = 0;
				Print(str);
		}
	}

	return 1;
}

int cMapInfo(void) {
	char str1[40], str2[40];
	int i;

	if (MapIsLoaded) {
		Print("----------MAPINFO----------");

		strcpy(str1, "FILE: ");
		for (i = 0; i < strlen(CurrentMap.Filename); i++) {
			str2[i] = toupper(CurrentMap.Filename[i]);
		}
		str2[i] = 0;
		strcat(str1, str2);
		Print(str1);

		strcpy(str1, "SIZEX: ");
		itoa(CurrentMap.MapInfo.SizeX, str2, 10);
		strcat(str1, str2);
		Print(str1);

		strcpy(str1, "SIZEY: ");
		itoa(CurrentMap.MapInfo.SizeY, str2, 10);
		strcat(str1, str2);
		Print(str1);

		strcpy(str1, "CELLSIZE: ");
		itoa(CurrentMap.MapInfo.CellSize, str2, 10);
		strcat(str1, str2);
		Print(str1);

		strcpy(str1, "NUMPLAYERS: ");
		itoa(CurrentMap.MapInfo.NumPlayers, str2, 10);
		strcat(str1, str2);
		Print(str1);

		strcpy(str1, "NUMUNITS: ");
		itoa(CurrentMap.MapInfo.NumUnits, str2, 10);
		strcat(str1, str2);
		Print(str1);

		strcpy(str1, "NUMBUILDINGS: ");
		itoa(CurrentMap.MapInfo.NumBuildings, str2, 10);
		strcat(str1, str2);
		Print(str1);

		strcpy(str1, "NAME: ");
		for (i = 0; i < strlen(CurrentMap.MapInfo.MapName); i++) {
			str2[i] = toupper(CurrentMap.MapInfo.MapName[i]);
		}
		str2[i] = 0;
		strcat(str1, str2);
		Print(str1);

		Print("---------------------------");
	}
	else {
		Print("NO DATA FOUND");
	}

	return 1;
}

int cFreeMap(void) {
	if (!MapIsLoaded) {
		Print("MAP IS NOT LOADED");
	}
	else {
		FreeMap();
		MapIsLoaded = 0;
		Print("MAP FREED");
	}

	return 1;
}

int cCenterMap(void) {
	char str1[32], str2[16];

	if (!MapIsLoaded) {
		Print("MAP IS NOT LOADED");
	}
	else {
		CurrentMap.Mx = ((CurrentMap.MapInfo.SizeX << MAP_SHIFT) >> 1)
			- (320 - 80) / 2;
		CurrentMap.My = ((CurrentMap.MapInfo.SizeY << MAP_SHIFT) >> 1)
			- 200 / 2;

		strcpy(str1, "CURRENTMAP.MX = ");
		itoa(CurrentMap.Mx, str2, 10);
		strcat(str1, str2);
		Print(str1);

		strcpy(str1, "CURRENTMAP.MY = ");
		itoa(CurrentMap.My, str2, 10);
		strcat(str1, str2);
		Print(str1);
	}

	return 1;
}

int cDump(void) {
	int r;
	static int img_num;
	char filename[32], tmpstr[16];

	itoa(img_num, tmpstr, 10);
	strcpy(filename, "");
	for (r = 0; r < 8 - strlen(tmpstr); r++) {
		strcat(filename, "0");
	}
	strcat(filename, tmpstr);

	if (DumpFormat == 1) {
		strcat(filename, ".BMP");
		r = SaveBMP(filename, FrameBuffer, MainPalette, 320, 200);
	}
	else {
		strcat(filename, ".PCX");
		r = SavePCX(filename, FrameBuffer, MainPalette, 320, 200);
	}

	if (!r) {
		Print("DUMP ERROR");
	}
	else {
		strcat(filename, " DUMPED");
		Print(filename);
		img_num++;
	}

	return 1;
}

int cSetVar(void) {
	VAR *v = VarList;
	int i, found, t, tt;
	char str1[32], str2[16];

	if (strcmp(GlobalArg1, "") == 0 ||
		strcmp(GlobalArg2, "") == 0) {
		Print("USAGE: SET VARIABLE_NAME VALUE");
		return 1;
	}

	found = 0;

	for (i = 0; i < MAX_VARS; i++, v++) {
		if (strcmp(GlobalArg1, v->VarName) == 0) {
			found = 1;
			break;
		}
	}

	if (!found) {
		Print("VARIABLE NOT FOUND");
	}
	else {
		if (!IsInt(GlobalArg2)) {
			Print("VALUE IS NOT A NUMBER");
		}
		else {
			t = atoi(GlobalArg2);

			if (t < v->Min || t > v->Max) {
				strcpy(str1, "RANGE ERROR. VAR = ");
				itoa(v->Min, str2, 10);
				strcat(str1, str2);
				strcat(str1, "...");
				itoa(v->Max, str2, 10);
				strcat(str1, str2);
				Print(str1);
			}
			else {
				*v->PtrToVar = t;

				strcpy(str1, GlobalArg1);
				strcat(str1, " SET TO ");
				strcat(str1, GlobalArg2);
				Print(str1);

				if (v->Proc) v->Proc();
			}
		}
	}

	return 1;
}

int cGetVar(void) {
	VAR *v = VarList;
	int i, found;
	char str1[32], str2[16];

	if (strcmp(GlobalArg1, "") == 0) {
		Print("USAGE: GET VARIABLE_NAME");
		return 1;
	}

	found = 0;

	for (i = 0; i < MAX_VARS; i++, v++) {
		if (strcmp(GlobalArg1, v->VarName) == 0) {
			found = 1;
			break;
		}
	}

	if (!found) {
		Print("VARIABLE NOT FOUND");
	}
	else {
		itoa(*v->PtrToVar, str2, 10);

		strcpy(str1, v->VarName);
		strcat(str1, " = ");
		strcat(str1, str2);
		Print(str1);
	}

	return 1;
}
