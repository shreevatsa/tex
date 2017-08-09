/* $Id: texexec.c,v 1.2 2004/03/30 14:57:06 taco Exp taco $ 
 *
 * This is a C port of Pragma's 'texexec.pl' perl script, v 4.0
 * 
 *
 * Copyright (c) 2004 Taco Hoekwater. taco@elvenkind.com
 *
 * License:  GPL version 2 (kpathsea version), or the ConTeXt
 *           license as defined in mreadme.pdf (standalone version)
 *
 * No Warranty whatsoever. Please report bugs, though.
 */

#include <stdlib.h>

#include <stdio.h>
#include <sys/types.h>

#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include <time.h>
#include <signal.h>

#include "perlemu.h"
#include "funclib.h"
#include "texutil.h"
#include "texexec.h"

#ifdef WIN32
#include <windows.h> /* for GetModuleFileName() */
#define setenv(a,b,c) {              \
        char *envval=NULL;           \
        my_concat3(&envval,a,"=",b); \
		putenv(envval);              \
        free(envval); }
#endif

#ifdef USE_KPSE
static const char *Program= " TeXExeck 4.0 - ConTeXt / PRAGMA ADE 1997-2004/C port";
#else
static const char *Program= " TeXExec 4.0 - ConTeXt / PRAGMA ADE 1997-2004/C port";
#endif

extern int RunTeX (char *a);


typedef struct option getoptstruct;

char *AddEmpty         = "";
int Alone            = 0;
int Optimize         = 0;
int ForceTeXutil     = 0;
int Arrange          = 0;
char *BackSpace        = "0pt";
char *Background       = "";
int CenterPage       = 0;
char *ConTeXtInterface = "unknown";
char *Convert          = "";
int DoMPTeX          = 0;
int DoMPXTeX         = 0;
int EnterMode        = 0;
char *Environments     = "";
char *Modules          = "";
int FastMode         = 0;
int FinalMode        = 0;
char *Format           = "";
char *MpDoFormat       = "";
int HelpAsked        = 0;
char *MainBodyFont     = "standard";
char *MainLanguage     = "standard";
char *MainResponse     = "standard";
int MakeFormats      = 0;
int Markings         = 0;
char *Mode             = "";
int NoArrange        = 0;
int NoDuplex         = 0;
int NOfRuns          = 7;
int NoMPMode         = 0;
int NoMPRun          = 0;
int NoBanner         = 0;
int AutoMPRun        = 0;
char *OutputFormat     = "standard";
char *Pages            = "";
char *PageScale        = "1000"; 
char *PaperFormat      = "standard";
char *PaperOffset      = "0pt";
char *PassOn           = "";
int PdfArrange       = 0;
int PdfSelect        = 0;
int PdfCombine       = 0;
int PdfOpen          = 0;
int PdfClose         = 0;
int AutoPdf          = 0;
char *PrintFormat      = "standard";
int ProducePdfT      = 0;
int ProducePdfM      = 0;
int ProducePdfX      = 0;
char *Input            = "";
char *Result           = "";
char *Suffix           = "";
int RunOnce          = 0;
char *Selection        = "";
char *Combination      = "2*4";
int SilentMode       = 0;
char *TeXProgram       = "";
char *TeXTranslation   = "";
char *TextWidth        = "0pt";
char *TopSpace         = "0pt";
int TypesetFigures   = 0;
int ForceFullScreen  = 0;
int ScreenSaver      = 0;
int TypesetListing   = 0;
int TypesetModule    = 0;
int UseColor         = 0;
int Verbose          = 0;
int PdfCopy          = 0;
char *LogFile          = "";
int MpyForce         = 0;
char *RunPath          = "";
char *Arguments        = "";
int Pretty           = 0;
char *SetFile          = "";
char *TeXTree          = "";
char *TeXRoot          = "";
int Purge            = 0;
char *Separation       = "";
char *ModeFile         = "";
int GlobalFile       = 0;
int AllPatterns      = 0;
int ForceXML         = 0;
int Random           = 0;
int RandomSeed       = 0;

char *Filters          = "";
int NoMapFiles       = 0;

char *MakeMpy = "";

struct option long_options[] = {
   { "arrange",        0, &Arrange, 1},
   { "batch",          0, &EnterMode, Batch},
   { "nonstop",        0, &EnterMode, NonStop},
   { "color",          0, &UseColor, 1},
   { "centerpage",     0, &CenterPage, 1},
   { "xml",            0, &ForceXML, 1},
   { "fast",           0, &FastMode, 1},
   { "final",          0, &FinalMode, 1},
   { "help",           0, &HelpAsked, 1},
   { "make",           0, &MakeFormats, 1},
   { "module",         0, &TypesetModule, 1},
   { "fullscreen",     0, &ForceFullScreen, 1},
   { "screensaver",    0, &ScreenSaver, 1},
   { "listing",        0, &TypesetListing, 1},
   { "mptex",          0, &DoMPTeX, 1},
   { "mpxtex",         0, &DoMPXTeX, 1},
   { "noarrange",      0, &NoArrange, 1},
   { "nomp",           0, &NoMPMode, 1},
   { "nomprun",        0, &NoMPRun, 1},
   { "nobanner",       0, &NoBanner, 1},
   { "automprun",      0, &AutoMPRun, 1},
   { "once",           0, &RunOnce, 1},
   { "pdf",            0, &ProducePdfT, 1},
   { "pdm",            0, &ProducePdfM, 1},
   { "pdx",            0, &ProducePdfX, 1},
   { "pdfarrange",     0, &PdfArrange, 1},
   { "pdfselect",      0, &PdfSelect, 1},
   { "pdfcombine",     0, &PdfCombine, 1},
   { "pdfcopy",        0, &PdfCopy, 1},
   { "noduplex",       0, &NoDuplex, 1},
   { "markings",       0, &Markings, 1},
   { "silent",         0, &SilentMode, 1},
   { "verbose",        0, &Verbose, 1},
   { "alone",          0, &Alone, 1},
   { "optimize",       0, &Optimize, 1},
   { "texutil",        0, &ForceTeXutil, 1},
   { "mpyforce",       0, &MpyForce, 1},
   { "pretty",         0, &Pretty, 1},
   { "purge",          0, &Purge, 1},
   { "random",         0, &Random, 1},
   { "allpatterns",    0, &AllPatterns, 1},
   { "pdfclose",       0, &PdfClose, 1},
   { "pdfopen",        0, &PdfOpen, 1},
   { "autopdf",        0, &AutoPdf, 1},
   { "globalfile",     0, &GlobalFile, 1},
   { "nomapfiles",     0, &NoMapFiles, 1},
   { "convert",      1,0,0}, //=> \$Convert,
   { "environments", 1,0,0}, //=> \$Environments,
   { "usemodules",   1,0,0}, //=> \$Modules,
   { "xmlfilters",   1,0,0}, //=> \$Filters,
   { "format",       1,0,0}, //=> \$Format,
   { "mpformat",     1,0,0}, //=> \$MpDoFormat,
   { "interface",    1,0,0}, //=> \$ConTeXtInterface,
   { "language",     1,0,0}, //=> \$MainLanguage,
   { "bodyfont",     1,0,0}, //=> \$MainBodyFont,
   { "results",      1,0,0}, //=> \$Result,
   { "response",     1,0,0}, //=> \$MainResponse,
   { "mode",         1,0,0}, //=> \$Mode,
   { "figures",      1,0,0}, //=> \$TypesetFigures,
   { "output",       1,0,0}, //=> \$OutputFormat,
   { "pages",        1,0,0}, //=> \$Pages,
   { "paper",        1,0,0}, //=> \$PaperFormat,
   { "passon",       1,0,0}, //=> \$PassOn,
   { "path",         1,0,0}, //=> \$RunPath,
   { "scale",        1,0,0}, //=> \$PageScale,
   { "selection",    1,0,0}, //=> \$Selection,
   { "combination",  1,0,0}, //=> \$Combination,
   { "paperoffset",  1,0,0}, //=> \$PaperOffset,
   { "backspace",    1,0,0}, //=> \$BackSpace,
   { "topspace",     1,0,0}, //=> \$TopSpace,
   { "textwidth",    1,0,0}, //=> \$TextWidth,
   { "addempty",     1,0,0}, //=> \$AddEmpty,
   { "background",   1,0,0}, //=> \$Background,
   { "logfile",      1,0,0}, //=> \$LogFile,
   { "print",        1,0,0}, //=> \$PrintFormat,
   { "suffix",       1,0,0}, //=> \$Suffix,
   { "runs",         1,0,0}, //=> \$NOfRuns,
   { "tex",          1,0,0}, //=> \$TeXProgram,
   { "input",        1,0,0}, //=> \$Input,
   { "arguments",    1,0,0}, //=> \$Arguments,
   { "setfile",      1,0,0}, //=> \$SetFile,
   { "makempy",      1,0,0}, //=> \$MakeMpy,
   { "separation",   1,0,0}, //=> \$Separation,
   { "textree",      1,0,0}, //=> \$TeXTree,
   { "texroot",      1,0,0}, //=> \$TeXRoot,
   { "translate",    1,0,0}, //=> \$TeXTranslation,
   { "modefile",     1,0,0}, //=> \$ModeFile,
   { 0,0,0,0}
};

char *TeXVirginFlag= "";
char *TeXPassString= "";
char *MpVirginFlag= "";
char *MpPassString= "";

char *TeXShell= "";
char *SetupPath= "";
char *UserInterface= "";
char *UsedInterfaces= "";
char *TeXFontsPath= "";
char *MpExecutable= "";
char *MpToTeXExecutable= "";
char *DviToMpExecutable= "";
char *TeXProgramPath= "";
char *TeXFormatPath= "";
char *ConTeXtPath= "";
char *TeXScriptsPath= "";
char *TeXHashExecutable= "";
char *TeXExecutable= "";
char *TeXBatchFlag= "";
char *TeXNonStopFlag= "";
char *MpBatchFlag= "";
char *MpNonStopFlag= "";
char *TeXFormatFlag= "";
char *MpFormatFlag= "";
char *MpFormat= "";
char *MpFormatPath= "";

char *FmtLanguage= "";
char *FmtBodyFont= "";
char *FmtResponse= "";
char *TcXPath= "";

char *TeXExec   = "texexec";
char *DVIspec   = "dvispec";
char *SGMLtoTeX = "sgml2tex";
char *FDFtoTeX  = "fdf2tex";
char *MetaFun   = "metafun";
char *MpToPdf   = "mptopdf";

FILE *STDOUT;
FILE *STDERR;


int Problems = 0;



struct locatedfile {
  char *path;
  char *name;
  char *file;
};


Hash ConTeXtInterfaces; 
Hash ResponseInterface;

Hash Help ;
int recurse = -1;
int shorthelp = 0;
int helpdone = 0;

struct helpopt {
  char *desc;
  Hash vals;
  Hash subs;
};


string opts[] = {
"arrange process and arrange",
"-----------",
"batch run in batch mode (don't pause)",
"-----------",
"nonstop run in non stop mode (don't pause)",
"-----------",
"centerpage center the page on the paper",
"-----------",
"color enable color (when not yet enabled)",
"-----------",
"usemodule load some modules first",
"=name list of modules",
"-----------",
"xmlfilter apply XML filter",
"=name list of filters",
"-----------",
"environment load some environments first",
"=name list of environments",
"-----------",
"fast skip as much as possible",
"-----------",
"figures typeset figure directory",
"=a room for corrections",
"=b just graphics",
"=c one (cropped) per page",
"paperoffset room left at paper border",
"fullscreen force full screen mode (pdf)",
"-----------",
"screensaver turn graphic file into a (pdf) full screen file",
"-----------",
"final add a final run without skipping",
"-----------",
"format fmt file",
"=name format file (memory dump)",
"-----------",
"mpformat mem file",
"=name format file (memory dump)",
"-----------",
"interface user interface",
"=en English",
"=nl Dutch",
"=de German",
"=cz Czech",
"=uk Brittish",
"=it Italian",
"-----------",
"language main hyphenation language",
"=xx standard abbreviation",
"-----------",
"listing produce a verbatim listing",
"backspace inner margin of the page",
"topspace top/bottom margin of the page",
"pretty enable pretty printing",
"color use color for pretty printing",
"-----------",
"make build format files",
"language patterns to include",
"bodyfont bodyfont to preload",
"response response interface language",
"format TeX format",
"mpformat MetaPost format",
"program TeX program",
"-----------",
"mode running mode",
"=list modes to set",
"-----------",
"module typeset tex/pl/mp module",
"-----------",
"mptex run an MetaPost plus btex-etex cycle",
"-----------",
"mpxtex generatet an MetaPostmpx file",
"-----------",
"noarrange process but ignore arrange",
"-----------",
"nomp don't run MetaPost at all",
"-----------",
"nomprun don't run MetaPost at runtime",
"-----------",
"automprun MetaPost at runtime when needed",
"-----------",
"once run TeX only once (no TeXUtil either)",
"-----------",
"output specials to use",
"=pdftex   Han The Than's pdf backend",
"=dvips    Thomas Rokicky's dvi to ps converter",
"=dvipsone YandY's dvi to ps converter",
"=dviwindo YandY's windows previewer",
"=dvipdfm  Mark Wicks' dvi to pdf converter",
"=dvipdfmx Jin-Hwan Cho's extended dvipdfm",
"-----------",
"passon switches to pass to TeX (--src for MikTeX)",
"-----------",
"pages pages to output",
"=odd odd pages",
"=even even pages",
"=x,y:z pages x and y to z",
"-----------",
"paper paper input and output format",
"=a4a3 A4 printed on A3",
"=a5a4 A5 printed on A4",
"-----------",
"path document source path",
"=string path",
"-----------",
"pdf produce PDF directly using pdf(e)tex",
"-----------",
"pdfarrange arrange pdf pages",
"paperoffset room left at paper border",
"paper paper format",
"noduplex single sided",
"backspace inner margin of the page",
"topspace top/bottom margin of the page",
"markings add cutmarks",
"background",
"=string background graphic",
"addempty add empty page after",
"textwidth width of the original (one sided) text",
"-----------",
"pdfcombine combine pages to one page",
"paperformat paper format",
"combination n*m pages per page",
"paperoffset room left at paper border",
"nobanner no footerline",
"-----------",
"pdfcopy scale pages down/up",
"scale new page scale",
"paperoffset room left at paper border",
"markings add cutmarks",
"background",
"=string background graphic",
"-----------",
"pdfselect select pdf pages",
"selection pages to select",
"=odd odd pages",
"=even even pages",
"=x,y:z pages x and y to z",
"paperoffset room left at paper border",
"paperformat paper format",
"backspace inner margin of the page",
"topspace top/bottom margin of the page",
"markings add cutmarks",
"background",
"=string background graphic",
"addempty add empty page after",
"textwidth width of the original (one sided) text",
"-----------",
"print page imposition scheme",
"=up 2 pages per sheet doublesided",
"=down 2 rotated pages per sheet doublesided",
"-----------",
"result resulting file",
"=name filename",
"-----------",
"input input file (if used)",
"=name filename",
"-----------",
"suffix resulting file suffix",
"=string suffix",
"-----------",
"runs maximum number of TeX runs",
"=n number of runs",
"-----------",
"silent minimize (status) messages",
"-----------",
"tex TeX binary",
"=name binary of executable",
"-----------",
"textree additional texmf tree to be used",
"=path subpath of tex root",
"-----------",
"texroot root of tex trees",
"=path tex root",
"-----------",
"verbose shows some additional info",
"-----------",
"help show this or more, e.g. '--help interface'",
"-----------",
"alone bypass utilities (e.g. fmtutil for non-standard fmt's)",
"-----------",
"texutil force TeXUtil run",
"-----------",
"setfile load environment (batch) file" ,
 ".",
"." };



int optsk=0;


Hash OutputFormats;

int ReportPath = 0;
int ReportName = 0;
int ReportFile = 1;;
char *LocatedPath = "";
char *LocatedName = "";


int FinalRunNeeded = 0;
char *own_path = "";
char *FullFormat = "";
FILE *OPT;
int UserFileOk = 0;
char *AllLanguages = "";

#define MATCH(a) (strstr(line,a) != NULL)


#define ARGUMENT_IS(a) STREQ (long_options[option_index].name, a)
#define ENDS_WITH(a,b) ((strlen(a)>=strlen(b)) && (strcasecmp(a+strlen(a)-strlen(b),b)==0))

int parse_options(int argc,  string * argv) {
  int g;   /* `getopt' return code.  */
  int option_index;
  opterr = 0;
  for (;;) {
    g = getopt_long (argc, argv, "+", long_options, &option_index);
    if (g == -1) /* End of arguments, exit the loop.  */
      break;
    if (g == '?') { /* Unknown option.  */
      break;
    }
   if (ARGUMENT_IS ( "convert"))            { Convert = optarg;
   } else if (ARGUMENT_IS ("environments")) { Environments = optarg;
   } else if (ARGUMENT_IS ("usemodules"))   { Modules = optarg;
   } else if (ARGUMENT_IS ("xmlfilters"))   { Filters = optarg;
   } else if (ARGUMENT_IS ("format"))       { Format = optarg;
   } else if (ARGUMENT_IS ("mpformat"))     { MpDoFormat = optarg;
   } else if (ARGUMENT_IS ("interface"))    { ConTeXtInterface = optarg;
   } else if (ARGUMENT_IS ("language"))     { MainLanguage = optarg;
   } else if (ARGUMENT_IS ("bodyfont"))     { MainBodyFont = optarg;
   } else if (ARGUMENT_IS ("results"))      { Result = optarg;
   } else if (ARGUMENT_IS ("response"))     { MainResponse = optarg;
   } else if (ARGUMENT_IS ("mode"))         { Mode = optarg;
   } else if (ARGUMENT_IS ("figures"))      { TypesetFigures = atoi(optarg);
   } else if (ARGUMENT_IS ("output"))       { OutputFormat = optarg;
   } else if (ARGUMENT_IS ("pages"))        { Pages = optarg;
   } else if (ARGUMENT_IS ("paper"))        { PaperFormat = optarg;
   } else if (ARGUMENT_IS ("passon"))       { PassOn = optarg;
   } else if (ARGUMENT_IS ("path"))         { RunPath = optarg;
   } else if (ARGUMENT_IS ("scale"))        { PageScale = optarg;
   } else if (ARGUMENT_IS ("selection"))    { Selection = optarg;
   } else if (ARGUMENT_IS ("combination"))  { Combination = optarg;
   } else if (ARGUMENT_IS ("paperoffset"))  { PaperOffset = optarg;
   } else if (ARGUMENT_IS ("backspace"))    { BackSpace = optarg;
   } else if (ARGUMENT_IS ("topspace"))     { TopSpace = optarg;
   } else if (ARGUMENT_IS ("textwidth"))    { TextWidth = optarg;
   } else if (ARGUMENT_IS ("addempty"))     { AddEmpty = optarg;
   } else if (ARGUMENT_IS ("background"))   { Background = optarg;
   } else if (ARGUMENT_IS ("logfile"))      { LogFile = optarg;
   } else if (ARGUMENT_IS ("print"))        { PrintFormat = optarg;
   } else if (ARGUMENT_IS ("suffix"))       { Suffix = optarg;
   } else if (ARGUMENT_IS ("runs"))         { NOfRuns = atoi(optarg);
   } else if (ARGUMENT_IS ("tex"))          { TeXProgram = optarg;
   } else if (ARGUMENT_IS ("input"))        { Input = optarg;
   } else if (ARGUMENT_IS ("arguments"))    { Arguments = optarg;
   } else if (ARGUMENT_IS ("setfile"))      { SetFile = optarg;
   } else if (ARGUMENT_IS ("makempy"))      { MakeMpy = optarg;
   } else if (ARGUMENT_IS ("separation"))   { Separation = optarg;
   } else if (ARGUMENT_IS ("textree"))      { TeXTree = optarg;
   } else if (ARGUMENT_IS ("texroot"))      { TeXRoot = optarg;
   } else if (ARGUMENT_IS ("translate"))    { TeXTranslation = optarg;
   } else if (ARGUMENT_IS ("modefile"))     { ModeFile = optarg;
   } /* Else it was a flag; getopt has already done the assignment.  */
  }
  return optind;
}

string *make_paths (char *envpath) {
  char *val;
  char *origpath;
  int k=0;
  char *test = ":";
  if (envpath == NULL) {
	origpath = safe_strdup("");
  } else {
	origpath = safe_strdup(envpath);
  }
  string *p_array = safe_malloc(strlen(envpath)+1);
  if (strchr(envpath,';') != NULL)
	test = ";";
  while ((val = next_word(&origpath,test))!=NULL) {
	p_array[k++] = val;
  }
  p_array[k++] = origpath;
  p_array[k] = NULL;
  return p_array;
}


void sig_ignore (int thesignal) {
  signal(thesignal,sig_ignore);
}

char *found_ini_file (char *suffix){
  char *filename = NULL;
  my_concat(&filename,"texexec.", suffix);
  return discover_file(filename,"other text files");
}

void CheckPath (char *Key, char *Value){
  if (Value == NULL || strlen(Value)==0)
	return;
  if ((strchr(Value,'/')!=NULL) && (strchr(Value,';')==NULL)) {
	Value = checked_path(strdup(Value)); 
	if(!dir_exists(Value)) {
	  fprintf(stdout,"                 error : %s set to unknown path %s\n", Key,Value);
	}
  }
}

int CompFileName (const void *a, const void *b) {
  return strcmp(*((const char **)a), *((const char **)b));
}

int CheckInputFiles (char *Input,string **Files) {
  int NOfFiles = globfiles(Input,Files,GLOB_DIRS|GLOB_DIRS);  
  if (NOfFiles>1)
    qsort(*Files,NOfFiles,sizeof(char *),&CompFileName);
  return NOfFiles;
}

/* width-first search */
struct locatedfile file_find (char *filename, char *pathtostarton ) {
  struct locatedfile foundfile = { "" , "", "" };
  string *Files;
  char *glob = NULL;
  my_concat(&glob,pathtostarton, "*");
  CheckInputFiles(glob,&Files);  
  int k=0;
  char *file;
  while ((file = Files[k++]) != NULL) {
	if (*(file+strlen(file)-1)!=pathslash) {
	  char *filepart = strrchr(file,pathslash)+1;
	  if (STREQ(filepart,filename)) { 
		/* we are done */
		foundfile.name = filepart;
		foundfile.path = pathtostarton;
		foundfile.file = file;
		return foundfile ;
	  }
	}
  }
  k=0;
  while ((file = Files[k++]) != NULL) {
	if (*(file+strlen(file)-1)==pathslash) {
	  /* now go down ... */
	  foundfile = file_find (filename, file);
	  if (strlen(foundfile.file))
		return foundfile;
	}
  }
  return foundfile;
}

struct locatedfile LocatedFile(char *PathToStartOn, char *FileToLocate , char *cur_path) {
  char *testfile=NULL;
  struct locatedfile foundfile = { "" , "", ""};
  my_concat(&testfile,cur_path,FileToLocate);
  if (file_exists(testfile)) {
	foundfile.path = strdup(cur_path);
	foundfile.name = strdup(FileToLocate);
	foundfile.file = testfile; 
 } else {
	safe_free (testfile);
	testfile=NULL;
	my_concat(&testfile,checked_path(PathToStartOn),FileToLocate);
	if (file_exists(testfile)) {
	  foundfile.path = strdup(checked_path(PathToStartOn));
	  foundfile.name = strdup(FileToLocate);
	  foundfile.file = testfile;
	} else {
	  safe_free (testfile);
	  testfile=NULL;
	  testfile =checked_path(PathToStartOn);
	  char *savedroot=NULL;
	  if (((savedroot = strcasestr(testfile, "/texmf/"))!=NULL) || 
		  ((savedroot = strcasestr(testfile, "\\texmf\\"))!=NULL)) {
		foundfile = file_find(FileToLocate,testfile);
		if (!strlen(foundfile.file)) {
		  foundfile = file_find(FileToLocate,savedroot);
		}
	  } else {
		foundfile = file_find(FileToLocate,testfile);
	  }
	}
  }
  safe_free (testfile);
  return foundfile;
}

void read_ini_file (Hash Done, char *IniPath, int Verbose){
  char *line;
  char *zline;
  FILE *INI;
  char *one=NULL;
  char *two=NULL;
  char *three=NULL;
  safe_fopen(INI,IniPath,"r");
  if (INI) {
    zline = malloc(sizeof(char *)*BUFSIZE);
    if (Verbose) { fprintf(stdout,"               reading : %s\n",IniPath); }
    while (readline(INI,&zline)) {
      line = safe_strdup(zline);
      if (!(isspace(line[0])||isalpha(line[0]))) { 
	/* ignore all lines that do not start with whitespace or alphabetic*/
      } else {
	int type = 0;
	one=NULL;
	two=NULL;
	three=NULL;
	while (isspace(*line)) line++;
	if (strncasecmp(line,"except for",strlen("except for"))==0) {
	  line += strlen("except for");
	  type = 1;
	} else if (strncasecmp(line,"for",strlen("for"))==0) {
	  line += strlen("for");
	  type = 2;
	}
	if (type) {
	  while (isspace(*line)) line++;
	  one = next_word(&line,"set");
	  if (one)
	    one = trim(one);
	}
	while (isspace(*line)) line++;
	if (one || (strncasecmp(line,"set",strlen("set"))==0)) {
	  if (!one)
	    line += strlen("set");
	  while (isspace(*line)) line++;
	  two = next_word(&line,"to");
	  if(two) {
	    two = trim(two);
	    line += strlen("to");
	    while (isspace(*line)) line++;
	    three = line;
	    if (*three)
	      three = trim(three);
	  }
	}
	if (!(two && three)) {
	  continue;
	}
	if (three[0] == '\'' || three[0] == '"')
	  three++;
	if (*(three+strlen(three)) == '\'' || *(three+strlen(three)) == '"')
	  *(three+strlen(three)) = 0;
	if (type == 1) {
	  if (strcmp(one,hash_value(Done,"TeXShell")) != 0) {
	    if (Verbose)
	      fprintf(stdout, "               setting : '%s' to '%s' except for '%s'\n",two,three,one);
	    set_value(Done,two,three);
	    CheckPath(two, three );
	  }
        } else if (type == 2) {
	  if (strcmp(one,hash_value(Done,"TeXShell")) == 0) {
	    if (Verbose)
	      fprintf(stdout,"               setting : '%s' to '%s' for '%s'\n",two,three,one);
	    set_value(Done,two,three);
	    CheckPath(two, three );
	  }
        } else {
	  if (get_item(Done,two) == NULL ) {
	    if (Verbose) 
	      fprintf(stdout,  "               setting : '%s' to '%s' for 'all'\n",two,three);
	    set_value(Done,two,three);
	    CheckPath(two, three );
	  }
        }
      }
    }
    safe_free(zline);
    safe_fclose(INI);
    if (Verbose) { fprintf(stdout, "\n"); }
  } else if (Verbose) {
    fprintf(stdout,"               warning : %s not found, did you read 'texexec.rme'?\n",IniPath);
    exit (1);
  } else {
    fprintf(stdout,"               warning : %s not found, try 'texexec --verbose'\n",IniPath);
    exit (1);
  }
}


char *IniValue (Hash Done, char *Key, char *Default) {
  char *ret = Default;
  if (get_item(Done, Key)) { ret = safe_strdup(hash_value(Done,Key)); }
  if (Verbose) { fprintf(stdout, "          used setting : %s = %s\n", Key, ret); }
  return ret;
}


/*  format H1 = @>>>>>>>>>>>>>>>>>>>>>   @<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//  format H2 =                           @<<<<<<<< : @<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//  format H3 = @>>>>>>>>>>>>>>>>>>>>>   @<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
*/

void p_write (int format, char *arga, char *argb) {
  if (format == 1) {
    fprintf(stdout,"%22.22s   %-45.45s\n",arga,argb);
  } else if (format == 2) {
    fprintf(stdout,"                           %-9.9s : %-33.33s\n",arga,argb);
  } else if (format == 3) {
    fprintf(stdout,"%22.22s   %-45.45s\n",arga,argb);
  }
}

void print_val (char *k, char *opt){
  p_write(2,k,opt);
}


void print_subopt (char *k, struct helpopt *opt ) {
  char *val = NULL;
  my_concat(&val,"--",k);
  p_write(3,val,opt->desc);
  string *the_keys;
  int numkeys = keys((Hash)opt->vals,&the_keys);
  int i;
  for (i=0;i<numkeys;i++) {
    char *k = the_keys[i];
    print_val(k, hash_value((Hash)opt->vals,k));
  }
}


void print_opt (char *k, struct helpopt *opt) {
  if (helpdone) { if(!shorthelp) fprintf(stdout, "\n"); }
  helpdone = 1; 
  char *val = NULL;
  my_concat(&val,"--",k);
  p_write(1,val,opt->desc);
  if (shorthelp < 0)
    return;
  string *the_keys;
  int numkeys = keys((Hash)opt->vals,&the_keys);
  int i;
  for (i=0;i<numkeys;i++) {
    char *k = the_keys[i];
    print_val(k, hash_value((Hash)opt->vals,k));
    safe_free(the_keys[i]);
  }
  safe_free(the_keys);
  if (shorthelp > 0)
    return;
  numkeys = keys((Hash)opt->subs,&the_keys);
  for (i=0;i<numkeys;i++) {
    char *k = the_keys[i];
    print_subopt( k, do_get_value((Hash)opt->subs,k) );
  }
}

void show_help_info (int numhelp,string *help) {
  int k;
  for (k=0;k<numhelp;k++) {
    print_opt( help[k], do_get_value(Help,help[k]));
  }
}

struct helpopt *read_options (char *v) {
  recurse++;
  v = chomp(v);
  struct helpopt *opt = malloc(sizeof(struct helpopt));
  opt->subs = make_hash();
  opt->vals = make_hash();
  opt->desc = safe_strdup(v);
  char *line;
  while (((line = opts[optsk++])!=NULL)&&line[0]!='.') {
    if (line[0] == '-') { 
      if (recurse) optsk--; 
      break; 
    }
    if (recurse && (line[0] != '-')) { 
      optsk--; break;
    }
    line = chomp(line);
    char *next = line;
    while (*next && !isspace(*next)) next++;
    char *kk = strndup(line,(next-line));
    while (isspace(*next)) next++;
    char *vv = "";
    if (*next)
      vv = next;
    if (strstr(line,"=") == line) { 
      set_value(opt->vals,kk,vv );
    } else if (!recurse) { 
      do_set_value(opt->subs,kk, read_options(vv));
    }
  }
  recurse--;
  return opt;
}

void read_all_options (void) {
  char *line;
  while (((line = opts[optsk++])!=NULL)&&line[0]!='.') {
    if (line[0] == '-') 
      break;
    char *next = line;
    while ((*next) && !(isspace(*next))) next++;
    char *k = strndup(line,(next-line));
    while (isspace(*next)) next++;
    char *v = "";
    if (*next) {
      v = next;
    }
    struct helpopt *ret = read_options(v);
    do_set_value(Help,k,(void *)ret);
  }
}


char *QuotePath (char *oirg ){
  char *path = safe_strdup(oirg);
  char *ret=NULL;
  char *e;
  while ((e = next_word(&path,","))) {
    if ((e[0]!= '"') && (strchr(e,' ')!=NULL))
      my_concat3(&e ,"\"", e, "\"");
    if (ret) {
      my_concat3(&ret,ret,",",e);
    } else {
      ret=strdup(e);
    }
  }
  if (ret) {
    my_concat3(&ret,ret,",",path);
  } else {
    ret=strdup(path);
  }
  return ret;
}

void SetInterfaces (char *shortv, char *longv,  char *full) {
  set_value(ConTeXtInterfaces, shortv, shortv);
  set_value(ConTeXtInterfaces,longv,shortv);
  set_value(ResponseInterface,shortv,full);
  set_value(ResponseInterface,longv,full);
}

void show_help_options (void) {
  fprintf(stdout,
      "                --help   overview of all options and their values\n"
      "            --help all   all about all options\n"
      "          --help short   just the main options\n"
      "   --help mode ... pdf   all about a few options\n"
      "        --help '*.pdf'   all about options containing 'pdf'\n");
}

char *F_MPJobName (char *JobName, char *MPfile) {
  char *MPJobName;
  char *jobmpfile =NULL;
  char *mpfile = NULL;
  my_concat(&mpfile,MPfile,".mp");
  my_concat3(&jobmpfile,JobName,"-",mpfile);
  if (file_exists(jobmpfile) && file_size(jobmpfile)  > 100 ) {
    MPJobName = jobmpfile;
  } else if (file_exists(mpfile) && file_size(mpfile)  > 100 ) {
    MPJobName = mpfile;
  } else { 
    MPJobName = safe_strdup("");
  }
  return MPJobName;
}

void RunPerlScript ( char *ScriptName, char *Options ) {
   char *cmd = NULL;
   if (dosish) {
     char *mytest = NULL;
     char *own_quote = "";
     if ((own_path[0]!= '"') && (strchr(own_path,' ')!=NULL))
       own_quote = "\"";
     my_concat(&mytest,own_path,ScriptName);
     if (file_exists(mytest)) {
       if (strlen(own_quote))
	 my_concat3(&mytest,own_quote,mytest,own_quote);
       my_concat3(&cmd,mytest," ",Options);
     } else {
       safe_free(mytest);
       my_concat(&mytest,TeXScriptsPath,ScriptName);
       if (file_exists(mytest)) {
	 if (strlen(own_quote))
	   my_concat3(&mytest,own_quote,mytest,own_quote);
	 my_concat3(&cmd,mytest," ",Options);
       } else {
	 cmd = safe_strdup("");
       }
     }
   } else {
     my_concat3(&cmd,ScriptName," ",Options);
   }
   if (strlen(cmd)) { system(cmd);  }
}

void ConvertXMLFile (char *FileNameOrig){
  char *FileName=NULL;
  my_concat(&FileName,FileNameOrig,".xml" );
  RunPerlScript( SGMLtoTeX, FileName);
}

void ConvertSGMLFile (char *FileNameOrig){
  char *FileName=NULL;
  my_concat(&FileName,FileNameOrig,".sgm" );
  RunPerlScript( SGMLtoTeX, FileName);
}


void  CheckOutputFormat (void) {
  int Ok = 1;
  char *a = NULL;  
  char *b = NULL;
  if (!STREQ(OutputFormat,"standard")) {
    char *F;
    char *zOutputFormat = safe_strdup(OutputFormat);
    while((F = words(&zOutputFormat,","))) {
      if (get_item(OutputFormats, lc (F ))) {
	char *OF = hash_value(OutputFormats, lc (F));
	my_concat3(&a,",",FullFormat,",");
	my_concat3(&b,",",OF,",");
	if ( strstr(a,b)!=NULL) {
	  continue;
	}
	if (strlen(FullFormat)) { my_concat(&FullFormat, FullFormat, ","); }
	my_concat(&FullFormat,FullFormat,hash_value(OutputFormats,lc(F)));
      } else {
	Ok = 0;
      }
    }
    if (!Ok) {
      fprintf(stdout,"%s",hash_value(Help,"output"));
    } else if (strlen(FullFormat)) {
      fprintf(OPT,"\\setupoutput[%s]\n", FullFormat);
    }
  }
  if (!strlen(FullFormat)) { FullFormat = OutputFormat; }
}





void MakeOptionFile (int FinalRun, int FastDisabled, char *JobName, char *JobSuffix, int KindOfRun ) {
  FILE *OPT;
  char *filename = NULL;
  my_concat(&filename,JobName,".top");
  safe_fopen(OPT, filename , "w");
  fprintf( OPT, "%% %s\n", filename);
  fprintf( OPT, "\\unprotect\n");
  if (strlen(ModeFile))    { fprintf(OPT, "\\readlocfile{%s}{}{}\n", ModeFile); }
  if (strlen(Result))      { fprintf(OPT, "\\setupsystem[file=%s]\n", Result); }
  else if (strlen(Suffix)) { fprintf(OPT, "\\setupsystem[file=%s%s]\n", JobName,Suffix); }
  if (strlen(RunPath)) {
    RunPath = unix_path(RunPath);
    RunPath = chop(RunPath);
    fprintf(OPT, "\\usepath[%s]\n", RunPath);
  }
  MainLanguage = lc (MainLanguage);
  if (!STREQ(MainLanguage,"standard")) {
    fprintf(OPT, "\\setuplanguage[%s]\n", MainLanguage);
  }
  if (!strcasestr(TeXShell,"MikTeX")) {
    fprintf(OPT, "\\def\\MPOSTbatchswitch   {%s}", MpBatchFlag);
    fprintf(OPT, "\\def\\MPOSTnonstopswitch {%s}", MpNonStopFlag);
    fprintf(OPT, "\\def\\MPOSTformatswitch  {%s %s}",MpPassString,MpFormatFlag);
  }
  if (!STREQ(FullFormat,"standard")) {
    fprintf(OPT,"\\setupoutput[%s]\n", FullFormat);
  }
  if (EnterMode==Batch)   { fprintf( OPT, "\\batchmode\n"); }
  if (EnterMode==NonStop) { fprintf( OPT, "\\nonstopmode\n"); }
  if (UseColor)         { fprintf( OPT, "\\setupcolors[\\c!status=\\v!start]\n"); }
  if (NoMPMode || NoMPRun || AutoMPRun ) {
    fprintf (OPT, "\\runMPgraphicsfalse\n");
  }
  if ((FastMode) && (!FastDisabled)) { fprintf(OPT, "\\fastmode\n"); }
  if (SilentMode) { fprintf (OPT, "\\silentmode\n"); }
  if (strlen(Separation)) {
    fprintf(OPT,"\\setupcolors[\\c!splitsen=%s]\n",Separation);
  }
  if (strlen(SetupPath)) { fprintf(OPT,"\\setupsystem[\\c!gebied={%s}]\n",SetupPath); }
  fprintf (OPT, "\\setupsystem[\\c!n=%d]\n",KindOfRun);

  if (!(PdfSelect) || (PdfCombine) || (PdfCopy) ) {
    if      (strcasestr(PaperFormat,"a4a3"))  { fprintf (OPT, "\\setuppapersize[A4][A3]\n"); }
    else if (strcasestr(PaperFormat,"a5a4"))  { fprintf (OPT, "\\setuppapersize[A5][A4]\n"); }
    else if (!STREQ(PaperFormat,"standard")) {
      char *zPaperFormat = safe_strdup(PaperFormat);
      char *runner = zPaperFormat;
      while (*runner) {
	if (*runner == 'x' || *runner == 'X') 
	  *runner = '*';
	runner++;
      }
      runner = zPaperFormat;
      zPaperFormat = uc (zPaperFormat);
      char *from = next_word(&runner,"*");
      char *to = runner;
      if (!(to && strlen(to))) { to = from; }
      fprintf( OPT, "\\setuppapersize[%s][%s]\n", from,to);
    }
  }
  if (( PdfSelect || PdfCombine || PdfCopy || PdfArrange ) && ( strlen(Background) ) ) {
    fprintf(stdout, "    background graphic : %s\n",Background);
    fprintf(OPT, "\\defineoverlay[whatever][{\\externalfigure[%s][\\c!factor=\\v!max]}]\n",Background);
    fprintf(OPT, "\\setupbackgrounds[\\v!pagina][\\c!achtergrond=whatever]\n");
  }
  if (CenterPage) {
    fprintf(OPT,"\\setuplayout[\\c!plaats=\\v!midden,\\c!markering=\\v!aan]\n");
  }
  if (NoMapFiles) {
    fprintf (OPT, "\\disablemapfiles\n");
  }
  if (NoArrange) { fprintf(OPT, "\\setuparranging[\\v!blokkeer]\n"); }
  else if ( Arrange || PdfArrange ) {
    FinalRunNeeded = 1;
    if (FinalRun) {
      char *DupStr;
      if (NoDuplex) { DupStr = ""; }
      else { DupStr = ",\\v!dubbelzijdig"; }
      if (!strlen(PrintFormat)) {
	fprintf(OPT, "\\setuparranging[\\v!normaal]\n");
      } else if (strcasestr(PrintFormat,"up")) {
	fprintf(OPT, "\\setuparranging[2UP,\\v!geroteerd%s]\n",DupStr);
      } else if (strcasestr(PrintFormat,"down")) {
	fprintf(OPT, "\\setuparranging[2DOWN,\\v!geroteerd%s]\n",DupStr);
      } else if (strcasestr(PrintFormat,"side")) {
	fprintf(OPT, "\\setuparranging[2SIDE,\\v!geroteerd%s]\n",DupStr);
      } else {
	fprintf(OPT,"\\setuparranging[%s]\n",PrintFormat);
      }
    } else {
      fprintf(OPT, "\\setuparranging[\\v!blokkeer]\n");
    }
  }
  if (strlen(Arguments)) { fprintf(OPT,"\\setupenv[%s]\n",Arguments); }
  if (strlen(Input))     { fprintf(OPT,"\\setupsystem[inputfile=%s]\n",Input); }
  else                   { fprintf(OPT,"\\setupsystem[inputfile=%s.%s]\n",JobName,JobSuffix); }
  if (Random)            { fprintf(OPT,"\\setupsystem[\\c!willekeur=%d]\n",RandomSeed); }
  if (strlen(Mode))      { fprintf(OPT,"\\enablemode[%s]\n",Mode); }
  if (strlen(Pages))  {
    if (STREQ(lc(Pages),"odd")) {
      fprintf(OPT,"\\chardef\\whichpagetoshipout=1\n");
    } else if (STREQ(lc(Pages),"even")) {
      fprintf (OPT, "\\chardef\\whichpagetoshipout=2\n");
    } else {
      char *page;
      char *NewPages=NULL;
      while ((page = words(&Pages,","))) {
	if (strstr(page,":")!=NULL) {
	  int from = atoi(next_word(&page,":"));
	  int to   = atoi(page);
	  int i;
	  for (i = from ; i <= to ; i++ ) {
	    my_concat3(&NewPages,NewPages,Itoa(i),",");
	  }
	} else {
	  my_concat3(&NewPages,NewPages,page,",");
	}
      }
      NewPages = chop (NewPages);
      fprintf(OPT,"\\def\\pagestoshipout{%s}\\n",NewPages);
    }
  }
  fprintf(OPT, "\\protect\n");
  if (strlen(Filters)) {
    char *F;
    char *zFilters = safe_strdup(Filters);
    while ((F = words(&zFilters,","))) {
      fprintf(OPT,"\\useXMLfilter[%s]\n",F);
    }
  }
  if (strlen(Modules)) {
    char *M;
    char *zModules = safe_strdup(Modules);
    while ((M = words(&zModules,","))) {
      fprintf(OPT,"\\usemodule[%s]\n",M);
    }
  }
  if (strlen(Environments)) {
    char *E;
    char *zEnv = safe_strdup(Environments);
    while ((E = words(&zEnv,","))) {
      fprintf(OPT,"\\environment %s\n",E);
    }
  }
  safe_fclose(OPT);
}



void ReportUserFile (void) {
  if (!UserFileOk)
    return;
  fprintf(stdout, "\n");
  if (!STREQ(MainLanguage,"standard") ) {
    fprintf(stdout, "   additional patterns : %s\n", AllLanguages);
    fprintf(stdout, "      default language : %s\n", MainLanguage);
  }
  if (!STREQ(MainBodyFont,"standard")) {
    fprintf(stdout, "      default bodyfont : %s\n", MainBodyFont);
  }
}

void MakeUserFile (void) {
  UserFileOk = 0;
  FILE *USR;
  if (AllPatterns) {
    safe_fopen(USR, "cont-fmt.tex", "w");
    fprintf (USR, "\\preloadallpatterns\n");
  } else {
    if (STREQ(MainLanguage,"standard") && STREQ(MainBodyFont,"standard" ))
      return;
    fprintf(stdout, "   preparing user file : cont-fmt.tex\n");
    safe_fopen(USR, "cont-fmt.tex", "w");
    fprintf(USR,"\\unprotect\n");
    AllLanguages = MainLanguage;
    if (!STREQ(MainLanguage,"standard")) {
      char *lang;
      char *MainLang = safe_strdup(MainLanguage);
      while ((lang = words(&MainLang,","))) {
	fprintf (USR,"\\installlanguage[\\s!%s][\\c!status=\\v!start]\n",lang);
      }
      MainLanguage = words(&MainLanguage,",");
      fprintf(USR,"\\setupcurrentlanguage[\\s!%s]\n", MainLanguage);
    }
    if (!STREQ(MainBodyFont,"standard")) {
      fprintf(USR, "\\definetypescriptsynonym[cmr][%s]", MainBodyFont);
      fprintf(USR, "\\definefilesynonym[font-cmr][font-%s]\n", MainBodyFont);
    }
    fprintf(USR,"\\protect\n");
  }
  fprintf(USR,"\\endinput\n");
  safe_fclose(USR);
  ReportUserFile();
  fprintf(stdout, "\n");
  UserFileOk = 1;
}

void RemoveResponseFile (void) { 
  unlink ("mult-def.tex");
}

void MakeResponseFile (void) {
  if (STREQ(MainResponse,"standard")) { RemoveResponseFile(); }
  else if (!get_item(ResponseInterface,MainResponse)) {
    RemoveResponseFile();
  } else {
    char *MR = hash_value(ResponseInterface, MainResponse);
    fprintf(stdout, "   preparing interface file : mult-def.tex\n");
    fprintf(stdout, "          response language : %s\n",MR);
    FILE *DEF;
    safe_fopen(DEF, "mult-def.tex", "w" );
    if (DEF) {
      fprintf(DEF, "\\def\\currentresponses{%s}\n\\endinput\n", MR);
      safe_fclose(DEF);
    }
  }
}

void RestoreUserFile (void) {
  unlink ("cont-fmt.log");
  rename ("cont-fmt.tex", "cont-fmt.log");
  ReportUserFile();
}

#define CheckPositions(a) ;


#define match_assign(var,word,value) {\
char *loc = strcasestr(line,word);    \
if (loc != NULL) {                    \
  loc++;                              \
  if (*loc == '=') {                  \
    loc++;                            \
    char *end = loc;                  \
    while ((c = *end) && value) {     \
      end++;                          \
    }                                 \
    var = safe_strdup(loc);           \
    var[(end-loc)] = 0;               \
  }                                   \
}}

void ScanPreamble (char *FileName, char **ConTeXtVersion, char **ConTeXtModes) {
  FILE *TEX;
  char *line;
  safe_fopen(TEX, FileName ,"r");
  if (TEX) {
    line = malloc(sizeof(char *)*BUFSIZE);
    char *zline = line;
    while (readline(TEX,&line)) {
      line = chomp(line);
      if (strlen(line) && line[0] == '%') {
	char c;
	match_assign(TeXExecutable,"tex",isalpha(c));
	match_assign(TeXTranslation,"translate-file",(isalpha(c)||isdigit(c)||c==':'||c=='/'||c=='-'));
	match_assign(TeXTranslation,"translate",(isalpha(c)||isdigit(c)||c==':'||c=='/'||c=='-'));
	match_assign(TeXExecutable,"program",isalpha(c));
	match_assign(OutputFormat,"output",(isalpha(c)||c==','||c=='-'));
	match_assign(*ConTeXtModes,"modes",(isalpha(c)||c==','||c=='-'));
	match_assign(TeXTree,"textree",(isalpha(c)||c=='-'));
	match_assign(TeXRoot,"texroot",(isalpha(c)||c=='-'));
	if (STREQ(ConTeXtInterface,"unknown")) {
	  char *tempi=NULL;
	  match_assign(tempi,"format",isalpha(c));
	  if (tempi && strlen(tempi)) {
	    ConTeXtInterface = hash_value(ConTeXtInterfaces,tempi);
	  }
	  match_assign(tempi,"interface",isalpha(c));
	  if (tempi && strlen(tempi)) {
	    ConTeXtInterface = hash_value(ConTeXtInterfaces,tempi);
	  }
	}
	match_assign(*ConTeXtVersion,"version",isalpha(c));
      } else {
	break;
      }
    }
    safe_free(zline);
    safe_fclose(TEX);
  }
}

void ScanContent (char *ConTeXtInput) {
  FILE *TEX;
  char *line;
  safe_fopen(TEX, ConTeXtInput,"r" );
  if (TEX) {
    line = malloc(sizeof(char *)*BUFSIZE);
    while (readline(TEX,&line)) {
      if (MATCH("\\starttekst")||
	  MATCH("\\stoptekst")||
	  MATCH("\\startonderdeel")||
	  MATCH("\\startdocument")||
	  MATCH("\\startoverzicht")){
	ConTeXtInterface = "nl"; break;
      } else if (MATCH("\\stelle")||
		 MATCH("\\verwende")||
		 MATCH("\\umgebung")||
		 MATCH("\\benutze")) {
	ConTeXtInterface = "de"; break;
      } else if (MATCH("\\stel")||MATCH("\\gebruik")||MATCH("\\omgeving")) {
	ConTeXtInterface = "nl"; break;
      } else if (MATCH("\\use")||MATCH("\\setup")||MATCH("\\environment")) {
	ConTeXtInterface = "en"; break;
      } else if (MATCH("\\usa")||MATCH("\\imposta")||MATCH("\\ambiente")) {
	ConTeXtInterface = "it"; break;
      } else if (MATCH("height=")||MATCH("width=")||MATCH("style=")) {
	ConTeXtInterface = "en"; break;
      } else if (MATCH("hoehe=")||MATCH("breite=")||MATCH("schrift=")) {
	ConTeXtInterface = "de"; break;
      } else if (MATCH("hoogte=")||MATCH("breedte=")||MATCH("letter="))  { 
	ConTeXtInterface = "nl"; break; 
      } else if (MATCH("altezza=")||MATCH("ampiezza=")||MATCH("stile=")) { 
	ConTeXtInterface = "it"; break; 
      } else if (MATCH("externfiguur"))              { 
	ConTeXtInterface = "nl"; break; 
      } else if (MATCH("externalfigure"))            { 
	ConTeXtInterface = "en"; break; 
      } else if (MATCH("externeabbildung"))          { 
	ConTeXtInterface = "de"; break; 
      } else if (MATCH("figuraesterna"))             { 
	ConTeXtInterface = "it"; break; 
      }
    }
    safe_free(line);
    safe_fclose(TEX);
  }
}

int SystemTeX (char *JobFile) {
  int StartTime = time(NULL);
  char *cmd=NULL;
  char *TeXProgNameFlag = "";
  int MyProblems = 0;
  if (!dosish) {
    TeXProgramPath = "";
    TeXFormatPath  = "";
    if (!TeXProgNameFlag && 
	(strstr(Format,"cont")==Format) && 
	(strcasestr(TeXPassString,"progname")==NULL)) {
      TeXProgNameFlag = "-progname=context";
    }
  }
  char *own_quote = "";
  if (TeXProgramPath[0] != '"' && (strchr(TeXProgramPath,' ')!=NULL))
    own_quote= "\"";
  char *exe= NULL;
  my_concat(&exe,TeXProgramPath,TeXExecutable);
  my_concat3(&exe,own_quote,exe,own_quote);
  my_concat3(&cmd,exe," ",TeXProgNameFlag);
  my_concat3(&cmd,cmd," ",TeXPassString);
  my_concat3(&cmd,cmd," ",PassOn);
  if (EnterMode==Batch)         { my_concat3(&cmd,cmd," ",TeXBatchFlag); }
  if (EnterMode==NonStop)       { my_concat3(&cmd,cmd," ",TeXNonStopFlag); }
  if (strlen(TeXTranslation)) { my_concat3(&cmd,cmd," -translate-file=",TeXTranslation); }
  my_concat3(&cmd,cmd," ", TeXFormatFlag);
  my_concat3(&cmd,cmd,TeXFormatPath,Format);
  my_concat3(&cmd,cmd," ", JobFile);
  if (Verbose)        { fprintf(stdout,"\n%s\n\n",cmd); }
  MyProblems = system(cmd);
  int StopTime = time(NULL) - StartTime;
  fprintf(stdout, "\n           return code : %d",MyProblems);
  fprintf(stdout, "\n              run time : %d seconds\n",StopTime);
  return MyProblems;
}


void F_CopyFile (char *From, char *To) { 
  FILE* INP;
  FILE *OUTP;
  char buf[(BUFSIZE+1)];
  safe_fopen(INP,From,"rb");
  if (!INP)
    return;
  safe_fopen(OUTP,To,"wb");
  if (!OUTP)
    return;
  while (fread((void *)buf,1,BUFSIZE,INP)) { fwrite((void *)buf,1,BUFSIZE,OUTP); }
  safe_fclose(INP);
  safe_fclose(OUTP);
}
/* return 1 if files differ */

int compare (char *ina, char *inb) {
  FILE* INA;
  FILE *INB;
  int a =0;
  int b =0;
  char bufa[(BUFSIZE+1)];
  char bufb[(BUFSIZE+1)];
  safe_fopen(INA,ina,"rb");
  if (!INA)
    return 1;
  safe_fopen(INB,inb,"rb");
  if (!INB)
    return 1;
  while((a = fread((void *)bufa,1,BUFSIZE,INA)) &&
	(b = fread((void *)bufb,1,BUFSIZE,INB))) {
    if (a != b)
      return 1;
    if (memcmp(bufa,bufb,a)!=0)
      return 1;
    if (a<BUFSIZE)
      return 0;
  }  
  return 0; 
}



void PushResult (char *File){
  char *tuo = NULL;
  char *file = NULL;
  if (strrchr(File,'.'))
    *(strrchr(File,'.'))=0;
  char *MyResult = safe_strdup(Result);
  if (strrchr(MyResult,'.'))
    *(strrchr(MyResult,'.'))=0;
  if (strlen(MyResult) && (!STREQ(MyResult,File))) {
    fprintf(stdout,"            outputfile : %s\n", MyResult);
    unlink ("texexec.tuo");
    my_concat(&file,File,".tuo");
    rename (file, "texexec.tuo");
    unlink ("texexec.log");
    my_concat(&file,File,".log");
    rename (file, "texexec.log");
    unlink ("texexec.dvi");
    my_concat(&file,File,".dvi");
    rename (file, "texexec.dvi");
    unlink ("texexec.pdf");
    my_concat(&file,File,".pdf");
    rename (file, "texexec.pdf");

    my_concat(&file,MyResult,".tuo");
    if (file_exists(file)) {
      unlink(tuo);
      rename (file,tuo);
    }
  }
  if (Optimize) {  my_concat(&tuo,File,".tuo");  unlink (tuo);  }
}

void PopResult (char *File){
  char *file = NULL;
  char *result = NULL;
  if (strrchr(File,'.'))
    *(strrchr(File,'.'))=0;
  char *MyResult = safe_strdup(Result);
  if (strrchr(MyResult,'.'))
    *(strrchr(MyResult,'.'))=0;
  if (strlen(MyResult) && (!STREQ(MyResult,File))) {
    fprintf(stdout, "              renaming : %s to %s\n", File, MyResult);
    my_concat(&file,File,".tuo");
    my_concat(&result,MyResult,".tuo");
    unlink (result);
    rename (file,result);
    my_concat(&file,File,".log");
    my_concat(&result,MyResult,".log");
    unlink (result);
    rename (file,result);
    my_concat(&file,File,".dvi");
    my_concat(&result,MyResult,".dvi");
    unlink (result);
    rename (file,result);
    if (file_exists(file)) { F_CopyFile( file, result ) ;}
    my_concat(&file,File,".pdf");
    my_concat(&result,MyResult,".pdf");
    unlink (result);
    rename (file,result);
    if (file_exists(file)) { F_CopyFile( file, result ) ;}
    if (!STREQ(File,"texexec" ))
      return;
    my_concat(&file,File,".tuo");
    rename ("texexec.tuo", file);
    my_concat(&file,File,".log");
    rename ("texexec.log", file);
    my_concat(&file,File,".dvi");
    rename ("texexec.dvi", file);
    my_concat(&file,File,".pdf");
    rename ("texexec.pdf", file);
  }
}

int RunTeXutil (char *JobName){
  int StopRunning;
  char *tup=NULL; 
  char *tuo = NULL;
  my_concat(&tuo,JobName,".tuo");
  my_concat(&tup,JobName,".tup");
  unlink (tup);
  rename (tuo,tup);
  fprintf(stdout, "  sorting and checking : running texutil\n");
  texutil my_texutil = texutil_new();
  my_texutil->TcXPath_field = TcXPath;
  my_texutil->ProcessReferences_field = 1;
  my_texutil->ProcessIJ_field = 1;
  my_texutil->ProcessHigh_field = 1;
  string newargv[1];
  newargv[0] = JobName;
  texutil_main(my_texutil,1,newargv);
  if (file_exists(tuo)) {
    CheckPositions(JobName);
    StopRunning = (compare(tup,tuo) ? 0 : 1);
  } else {
    StopRunning = 1;
  }
  if (!StopRunning) {
    fprintf(stdout,"\n utility file analysis : another run needed\n");
  }
  return StopRunning;
}


void PurgeFiles (char *JobName){
  fprintf (stdout,"\n         purging files : %s\n",JobName);
  texutil my_texutil = texutil_new();
  my_texutil->PurgeFiles_field = 1;
  string newargv[1];
  newargv[0] = JobName;
  texutil_main(my_texutil,1,newargv);
  char *file = NULL;
  my_concat(&file,Result,".log");
  if (file_exists(file))
    unlink(file);
}


int RunTeXMP (char *JobName, char *MPfile) {
  int MPrundone        = 0;
  char *MPJobName      = F_MPJobName( JobName, MPfile );
  char *MPFoundJobName = "";
  if (strlen(MPJobName)) {
    FILE *MP;
    safe_fopen(MP,MPJobName,"r");
    if (MP) {
      char *line = malloc(sizeof(char *)*BUFSIZE);
      readline(MP,&line);
      line = chomp(line);
      char c;
      match_assign(TeXTranslation,"translate-file",(isalpha(c)||isdigit(c)||c=='-'));
      match_assign(TeXTranslation,"translate",(isalpha(c)||isdigit(c)||c=='-'));
      if (strcasestr(line,"collected graphics of job \"")!=NULL) {
	line += strlen("collected graphics of job \"");
	MPFoundJobName = line;
	line = strchr(line,'"');
	*line = 0;
      }
      safe_fclose(MP);
      if (strlen(MPFoundJobName)) {
	if (strcasestr(JobName,MPFoundJobName)!=NULL) {
	  if (strlen(MpExecutable)) {
	    fprintf(stdout, "   generating graphics : metaposting %s\n",MPJobName);
	    char *ForceMpy = " ";
	    if (MpyForce) { ForceMpy = "--mpyforce "; }
	    char *ForceTCX = safe_strdup(" ");
	    if (strlen(TeXTranslation)) {
	      my_concat3(&ForceTCX,"--translate=",TeXTranslation," ");
	    }
	    char *options = NULL;
	    my_concat(&options,ForceTCX,ForceMpy);
	    if (EnterMode==Batch) {
	      my_concat3(&options,options,"--mptex --nomp --batch ",MPJobName);
	      RunPerlScript( TeXExec,options);
	    } else if (EnterMode==NonStop) {
	      my_concat3(&options,options,"--mptex --nomp --nonstop ",MPJobName);
	      RunPerlScript( TeXExec,options);
	    } else {
	      my_concat3(&options,options," ",MPJobName);
	      RunPerlScript( TeXExec,options);
	    }
	  } else {
	    fprintf(stdout, "   generating graphics : metapost cannot be run\n");
	  }
	  MPrundone = 1;
	}
      }
    }
  }
  return MPrundone;
}

/* perlversion  is broken: it does only each second file. */
unsigned CheckChanges (char *JobName) { 
  int checksum  = 0;
  char *MPJobName = F_MPJobName( JobName, "mpgraph" );
  FILE *MP;
  safe_fopen(MP,MPJobName,"r");
  if (MP) {
    char *line = safe_malloc(sizeof(char *)*BUFSIZE);
    while (readline(MP,&line)) {
      if (strcasestr(line,"random")==NULL) {
	checksum += (cksum(line) % 65535);
      }
    }
    safe_fclose(MP);
    safe_free(line);
  }
  MPJobName = F_MPJobName( JobName, "mprun" );
  safe_fopen(MP,MPJobName,"r");
  if (MP) {
    char *line = safe_malloc(sizeof(char *)*BUFSIZE);
    while (readline(MP,&line)) {
      if (strcasestr(line,"random")==NULL) {
	checksum += (cksum(line) % 65535);
      }
    }
    safe_fclose(MP);
    safe_free(line);
  }
  return checksum;
}


int isXMLfile (char *Name) {
  char *qtest = strrchr(Name,'.');
  if ((ForceXML) || STREQ(qtest,".xml")) { return 1; }
  else {
    FILE *XML;
    safe_fopen(XML, Name, "r");
    char buf[7];
    buf[6]=0;
    fread(buf,1,6,XML);
    safe_fclose(XML);
    return (strcasestr(buf,"<?xml ")==NULL ? 0 : 1);
  }
}

void RunConTeXtFile (char *JobName, char *JobSuffix ) {
  int Ok = 0;
  int DummyFile = 0;
  JobName = unix_path(JobName);
  RunPath = unix_path(RunPath);
  char *OriSuffix = safe_strdup(JobSuffix);
  if ((dosish) && (PdfClose)) {
    char *cmdline = NULL;
    char *pdffile = NULL;
    my_concat(&pdffile,JobName,".pdf");
    int ok = 0;
    if (file_exists(pdffile)) {
      my_concat3(&cmdline,"pdfclose --file ",JobName,".pdf");
      ok = system(cmdline);
    }
    my_concat(&pdffile,Result,".pdf");
    if ((strlen(Result)) && (file_exists(pdffile))) {
      my_concat3(&cmdline,"pdfclose --file ",Result,".pdf");
      ok = system(cmdline) ;
    }
    if(!ok)
      system("pdfclose --all");
  }
  char *jobfile = NULL;
  my_concat3(&jobfile,JobName,".",JobSuffix);
  if (file_exists(jobfile)) {
    DummyFile = ( (ForceXML) || (strcasestr(JobSuffix,"xml")!=NULL) );
  } else if (strlen(RunPath)) {
    char *rp;
    char *filename=NULL;
    while ((rp = words(&RunPath,","))) {
      my_concat3(&filename,rp,"/",jobfile);
      if (file_exists(filename)) { 
	DummyFile = 1; 
	break;
      }
    }
  }
  if (DummyFile) {
    FILE *TMP;
    my_concat(&jobfile,JobName,".run" );
    safe_fopen( TMP, jobfile, "w");
    if ((strcasestr(JobSuffix,"xml")!=NULL) || ForceXML ) {
      if (strlen(Filters)) {
	fprintf(stdout, "     using xml filters : %s\n",Filters);
      }
      fprintf( TMP, "\\starttext\n");
      fprintf( TMP, "\\processXMLfilegrouped{%s.%s}\n",JobName,JobSuffix);
      fprintf( TMP, "\\stoptext\n");
    } else {
      fprintf( TMP, "\\starttext\n");
      fprintf( TMP, "\\processfile{%s.%s}\n",JobName,JobSuffix);
      fprintf( TMP, "\\stoptext\n");
    }
    safe_fclose(TMP);
    JobSuffix = "run";
  }
  char *ConTeXtVersion = "unknown";
  char *ConTeXtModes   = "";
  if ( (file_exists(jobfile)) || (GlobalFile) ) {
    if (!DummyFile)  {
      ScanPreamble(jobfile,&ConTeXtVersion,&ConTeXtModes);
      if ( STREQ(ConTeXtInterface, "unknown" ) ) {
	ScanContent(jobfile);
      }
    }
    if (  STREQ(ConTeXtInterface, "unknown" ) ) {
      ConTeXtInterface = UserInterface;
    }
    if ( STREQ(ConTeXtInterface, "unknown" ) ) { ConTeXtInterface = "en"; }
    if ( STREQ(ConTeXtInterface, "") )         { ConTeXtInterface = "en"; }
    CheckOutputFormat();
    int StopRunning = 0;
    int MPrundone   = 0;
    if (!strlen(Format)) { Format = NULL; my_concat(&Format,"cont-",ConTeXtInterface); }
    fprintf(stdout, "            executable : %s%s\n",TeXProgramPath,TeXExecutable);
    fprintf(stdout, "                format : %s%s\n",TeXFormatPath,Format);
    if (strlen(RunPath)) { fprintf(stdout, "           source path : %s\n",RunPath); }
    if (DummyFile) {       fprintf(stdout, "            dummy file : %s\n",jobfile); }
    fprintf(stdout, "             inputfile : %s\n",JobName);
    fprintf(stdout, "                output : %s\n",FullFormat);
    fprintf(stdout, "             interface : %s\n",ConTeXtInterface);
    if (strlen(TeXTranslation)) {
      fprintf(stdout, "           translation : %s\n",TeXTranslation);
    }
    char *Options = safe_strdup("");
    if (Random)            { my_concat(&Options,Options," random");     }
    if (FastMode)          { my_concat(&Options,Options," fast");       }
    if (FinalMode)         { my_concat(&Options,Options," final");      }
    if (Verbose)           { my_concat(&Options,Options," verbose");    }
    if (TypesetListing)    { my_concat(&Options,Options," listing");    }
    if (TypesetModule)     { my_concat(&Options,Options," module");     }
    if (TypesetFigures)    { my_concat(&Options,Options," figures");    }
    if (MakeFormats)       { my_concat(&Options,Options," make");       }
    if (RunOnce)           { my_concat(&Options,Options," once");       }
    if (UseColor)          { my_concat(&Options,Options," color");      }
    if (EnterMode==Batch)  { my_concat(&Options,Options," batch");      }
    if (EnterMode==NonStop){ my_concat(&Options,Options," nonstop");    }
    if (NoMPMode)          { my_concat(&Options,Options," nomp");       }
    if (CenterPage)        { my_concat(&Options,Options," center");     }
    if (Arrange)           { my_concat(&Options,Options," arrange");    }
    if (NoArrange)         { my_concat(&Options,Options," no-arrange"); }

    if (strlen(Options))      { fprintf(stdout, "               options :%s\n",Options); }
    if (strlen(ConTeXtModes)) { fprintf(stdout, "        possible modes : %s\n",ConTeXtModes); }
    if (strlen(Mode))         { fprintf(stdout, "          current mode : %s\n",Mode); }
    else                      { fprintf(stdout, "          current mode : none\n"); }
    if (strlen(Arguments))    { fprintf(stdout, "             arguments : %s\n",Arguments); }
    if (strlen(Modules))      { fprintf(stdout, "               modules : %s\n",Modules); }
    if (strlen(Environments)) { fprintf(stdout, "          environments : %s\n",Environments); }
    if (strlen(Suffix))       { Result = NULL; my_concat(&Result,JobName,Suffix); }
    PushResult(JobName);
    Problems = 0;
    int TeXRuns = 0;
    if ( (PdfArrange) || (PdfSelect) || (RunOnce) ) {
      MakeOptionFile( 1, 1, JobName, OriSuffix, 3 );
      fprintf(stdout, "\n");
      char *JobFile = NULL;
      my_concat3(&JobFile,JobName,".",JobSuffix);
      Problems = RunTeX( JobFile );
      if (ForceTeXutil) { Ok = RunTeXutil(JobName); }
      char *topfile = NULL;
      my_concat(&topfile,JobName,".top");
      char *tmpfile = NULL;
      my_concat(&tmpfile,JobName,".tmp");
      F_CopyFile(topfile,tmpfile);
      unlink (topfile);   
      PopResult(JobName);
    } else {
      while ( (!StopRunning) && ( TeXRuns < NOfRuns ) && ( !Problems ) ) {
	++TeXRuns;
	if (TeXRuns == 1 ) {
	  MakeOptionFile( 0, 0, JobName, OriSuffix, 1 );
	} else {
	  MakeOptionFile( 0, 0, JobName, OriSuffix, 2 );
	}
	fprintf(stdout, "               TeX run : %d\n\n",TeXRuns);
	unsigned mpchecksumbefore = 0;
	unsigned mpchecksumafter = 0;
	if (AutoMPRun) { mpchecksumbefore = CheckChanges(JobName); }
	char *JobFile = NULL;
	my_concat3(&JobFile,JobName,".",JobSuffix);
	Problems = RunTeX( JobFile );
	if (AutoMPRun) { mpchecksumafter = CheckChanges(JobName); }
	if ( ( !Problems ) && ( NOfRuns > 1 ) ) {
	  if ( !NoMPMode ) {
	    MPrundone = RunTeXMP( JobName, "mpgraph" );
	    MPrundone = RunTeXMP( JobName, "mprun" );
	  }
	  StopRunning = RunTeXutil(JobName);
	  if (AutoMPRun) {
	    StopRunning =(StopRunning && ( mpchecksumafter == mpchecksumbefore ));
	  }
	}
      }
      if ( ( NOfRuns == 1 ) && ForceTeXutil ) {
	Ok = RunTeXutil(JobName);
      }
      if ( ( !Problems ) && ( ( FinalMode || FinalRunNeeded ) ) && ( NOfRuns > 1 ) ) {
	MakeOptionFile( 1, FinalMode, JobName, OriSuffix, 4 );
	fprintf(stdout, "         final TeX run : %d\n\n",TeXRuns);
	char *JobFile = NULL;
	my_concat3(&JobFile,JobName,".",JobSuffix);
	Problems = RunTeX( JobFile );
      }
      char *topfile = NULL;
      my_concat(&topfile,JobName,".top");
      char *tmpfile = NULL;
      my_concat(&tmpfile,JobName,".tmp");
      char *tupfile = NULL;
      my_concat(&tupfile,JobName,".tup");
      F_CopyFile(topfile,tmpfile);
      unlink (tupfile); 
      unlink (topfile);
      PopResult(JobName);
    }
    if (Purge) { PurgeFiles(JobName); }
    if (DummyFile) {
      unlink (jobfile);
    }
    if ((dosish) && (!Problems) && (PdfOpen)) {
      char *cmdline = NULL;
      if (strlen(Result)) {
	my_concat3(&cmdline,"pdfopen --file ",Result,".pdf");
      } else {
	my_concat3(&cmdline,"pdfopen --file ",JobName,".pdf");
      }
      system(cmdline);
    }
  }
}


void RunSomeTeXFile (char *JobName, char*JobSuffix){
  char *jobfile = NULL;
  my_concat3(&jobfile,JobName,".",JobSuffix);
  if (file_exists(jobfile)) {
    PushResult(JobName);
    fprintf(stdout, "            executable : %s%s\n",TeXProgramPath,TeXExecutable);
    fprintf(stdout, "                format : %s%s\n",TeXFormatPath,Format);
    fprintf(stdout, "             inputfile : %s\n", jobfile);
    Problems = RunTeX( jobfile );
    PopResult(JobName);
  }
}


void DoRunModule ( char *FileName, char *FileSuffix ) {
  char *ModuleFile  = "texexec";
  char *file = NULL;
  my_concat3(&file,FileName,".",FileSuffix);
  texutil my_texutil = texutil_new();
  my_texutil->ProcessDocuments_field = 1;
  string newargv[1];
  newargv[0] = file;
  texutil_main(my_texutil,1,newargv);
  //
  fprintf(stdout, "                module : %s\n\n",FileName);
  FILE *MOD;
  char *texfile = NULL;
  my_concat(&texfile,FileName,".tex");
  safe_fopen( MOD, texfile,"w");
  char *tedfile = NULL;
  my_concat(&tedfile,FileName,".ted");
  FILE *TED;
  safe_fopen( TED, texfile,"r");
  if (TED) {
    char *firstline = malloc(sizeof(char *)*BUFSIZE);
    readline(TED,&firstline);
    safe_fclose(TED);
    if (strstr(firstline,"/interface=en")!=NULL) { fprintf( MOD,"%s",firstline); }
    else { fprintf(MOD, "%% interface=nl\n"); }
  }
  fprintf( MOD, "\\usemodule[abr-01,mod-01]\n");
  fprintf( MOD, "\\def\\ModuleNumber{1}\n");
  fprintf( MOD, "\\starttekst\n");
  fprintf( MOD, "\\readlocfile{%s.ted}{}{}\n", FileName);
  fprintf( MOD, "\\stoptekst\n");
  safe_fclose(MOD);
  RunConTeXtFile( ModuleFile, "tex" );
  char *ms = NULL;
  char *fs = NULL;
  int k;
  if (!STREQ(FileName,ModuleFile)) {
    char *Suffixes[5] = {"dvi", "pdf", "tui", "tuo", "log" };
    for (k=0; k<5; k++) {
      my_concat3(&fs,FileName,".",Suffixes[k]); 
      my_concat3(&ms,ModuleFile,".",Suffixes[k]);
      unlink(fs); 
      rename(ms,fs);
    }
  }
  my_concat3(&ms,ModuleFile,".","tex");
  unlink(ms);
}

void RunModule (int Filenamecount,string *Filenames){
  int k;
  if (Filenamecount>1)
    qsort(Filenames,Filenamecount,sizeof(char *),&my_strcmp);
  if (!(file_exists(Filenames[0] ))) {
    char *Name = Filenames[0];
    char *Suffixes[4] ={".tex" , ".mp" , ".pl" , ".pm"};
    Filenames = safe_realloc(Filenames,sizeof(char *)*4);
    Filenamecount=0;
    for (k =0; k<4; k++) {
      char *file=NULL;
      my_concat(&file,Name,Suffixes[k]);
      Filenames[k] = file;
    }
  }
  for (k=0; k<Filenamecount;k++) {
    char *FileName = Filenames[k];
    if(!file_exists(FileName))
      continue;
    char *Name = next_word(&FileName,".");
    char *Suffix = FileName;
    if (!(strcasestr(Suffix,"tex")||
	  strcasestr(Suffix,"mp")||
	  strcasestr(Suffix,"pl")||
	  strcasestr(Suffix,"pm")))
      continue;
    DoRunModule( Name, Suffix );
  }
}

void RunFigures (int Filecount, string*Files){
  char *FiguresFile = "texexec";
  TypesetFigures = tolower (TypesetFigures);
  if (TypesetFigures < 'a' || TypesetFigures > 'd')
    return;
  char *pdffile=NULL;
  my_concat(&pdffile,FiguresFile,".pdf");
  unlink (pdffile);
  if (Filecount) { 
    texutil my_texutil = texutil_new();
    my_texutil->ProcessFigures_field = 1;
    texutil_main(my_texutil,Filecount,Files);
  }
  FILE *FIG;
  char *texfile=NULL;
  my_concat(&texfile,FiguresFile,".tex");
  safe_fopen(FIG, texfile,"w");
  if(FIG) {
    fprintf (FIG, "%% format=english\n");
    fprintf (FIG, "\\setuplayout\n");
    fprintf (FIG, "  [topspace=1.5cm,backspace=1.5cm,\n");
    fprintf (FIG, "   header=1.5cm,footer=0pt,\n");
    fprintf (FIG, "   width=middle,height=middle]\n");
    if (ForceFullScreen) {
      fprintf(FIG, "\\setupinteraction\n");
      fprintf(FIG, "  [state=start]\n");
      fprintf(FIG, "\\setupinteractionscreen\n");
      fprintf(FIG, "  [option=max]\n");
    }
    fprintf(FIG,"\\starttext\n");
    fprintf(FIG,"\\showexternalfigures[alternative=$TypesetFigures,offset=$PaperOffset]\n");
    fprintf(FIG,"\\stoptext\n");
    safe_fclose(FIG);
    ConTeXtInterface = "en";
    RunConTeXtFile( FiguresFile, "tex" );
    if (file_exists("texutil.tuf"))
      unlink("texutil.tuf");
  }
}

char *CleanTeXFileName (char *str){
  char *ret=malloc(strlen(str)*8);
  char *zret = ret;
  int k=0;
  while(str[k]) {
    if (str[k] == '$' || str[k] == '_' || str[k] == '_' || str[k] == '#' ) {
      *zret = '\\'; zret++;
    } else if (str[k] == '~' ) {
      *zret = '\\'; zret++;
      *zret = 's';  zret++;
      *zret = 't';  zret++;
      *zret = 'r';  zret++;
      *zret = 'i';  zret++;
      *zret = 'n';  zret++;
      *zret = 'g';  zret++;
    }
    *zret = str[k]; zret++; 
    k++;
  }
  *zret=0;
  return ret;
}


/* argument parsing is broken */
void RunListing (int FileNameCount,string *AllFileNames) {
  char *ListingFile = "texexec";
    char *FileName = AllFileNames[0];
    char *CleanFileName = FileName;
    string *FileNames;
    FileNameCount = CheckInputFiles(FileName,&FileNames);
    if (!file_exists(FileNames[0]))
      return;
    fprintf(stdout, "            input file : %s\n",FileName);
    if (STREQ(BackSpace,"0pt")) { BackSpace = "1.5cm"; }
    else { fprintf(stdout, "             backspace : %s\n",BackSpace); }
    if (STREQ(TopSpace,"0pt")) { TopSpace = "1.5cm"; }
    else { fprintf(stdout,"              topspace : %s\n",TopSpace); }
    FILE *LIS;
    char *texfile=NULL;
    my_concat(&texfile,ListingFile,".tex");
    safe_fopen(LIS, texfile,"w");
    if(LIS) {
      fprintf (LIS, "%% format=english\n");
      fprintf (LIS, "\\setupbodyfont[11pt,tt]\n");
      fprintf (LIS, "\\setuplayout\n");
      fprintf (LIS, "  [topspace=%s,backspace=%s,\n",TopSpace,BackSpace);
      fprintf (LIS, "   header=0cm,footer=1.5cm,\n");
      fprintf (LIS, "   width=middle,height=middle]\n");
      fprintf (LIS, "\\setuptyping[lines=yes]\n");
      if (Pretty) { fprintf (LIS, "\\setuptyping[option=color]\n"); }
      fprintf (LIS, "\\starttext\n");
      char *Filename;
      int k= 0;
      while ((Filename = FileNames[k++])) {
	CleanFileName = lc (CleanTeXFileName(Filename));
	fprintf (LIS, "\\page\n");
	fprintf (LIS, "\\setupfootertexts[%s][pagenumber]\n",CleanFileName);
	fprintf (LIS, "\\typefile{%s}\n",Filename);
      }
      fprintf (LIS, "\\stoptext\n");
      safe_fclose(LIS);
      ConTeXtInterface = "en";
      RunConTeXtFile( ListingFile, "tex" );
    }
}



void RunArrange (int noffiles, string *files){
  char *ArrangeFile = "texexec";
  fprintf(stdout, "             backspace : %s\n",BackSpace);
  fprintf(stdout, "              topspace : %s\n",TopSpace);
  fprintf(stdout, "           paperoffset : %s\n",PaperOffset);
  if (!strlen(AddEmpty)) { fprintf(stdout, "     empty pages added : none\n"); }
  else { fprintf(stdout, "     empty pages added : %s\n", AddEmpty); }
  if (STREQ(TextWidth,"0pt")) { fprintf(stdout, "             textwidth : unknown\n"); }
  else { fprintf(stdout, "             textwidth : %s\n",TextWidth); }
  char *arrfile = NULL;
  my_concat(&arrfile,ArrangeFile,".tex");
  FILE *ARR;
  safe_fopen( ARR,arrfile,"w" );
  if(ARR) {
    fprintf(ARR,  "%% format=english\n");
    fprintf(ARR,  "\\definepapersize\n");
    fprintf(ARR,  "  [offset=%s]\n",PaperOffset);
    fprintf(ARR,  "\\setuplayout\n");
    fprintf(ARR,  "  [backspace=%s,\n",BackSpace);
    fprintf(ARR,  "    topspace=%s,\n",TopSpace);
    if (Markings) {
      fprintf(ARR,  "     marking=on,\n");
      fprintf(stdout, "           cutmarkings : on\n");
    }
    fprintf(ARR,  "       width=middle,\n");
    fprintf(ARR,  "      height=middle,\n");
    fprintf(ARR,  "    location=middle,\n");
    fprintf(ARR,  "      header=0pt,\n");
    fprintf(ARR,  "      footer=0pt]\n");
    if (NoDuplex) { fprintf(stdout, "                duplex : off\n"); }
    else {
      fprintf(stdout, "                duplex : on\n");
      fprintf(ARR,  "\\setuppagenumbering\n");
      fprintf(ARR,  "  [alternative=doublesided]\n");
    }
    fprintf(ARR,  "\\starttext\n");
    int k = 0;
    for  (k=0; k<noffiles; k++) {
      char *FileName = files[k];
      fprintf(stdout, "               pdffile : %s\n",FileName);
      fprintf(ARR,  "\\insertpages\n  [%s]",FileName);
      if (strlen(AddEmpty)) { fprintf(ARR,  "[%s]",AddEmpty); }
      fprintf(ARR,  "[width=%s]\n",TextWidth);
    }
    fprintf(ARR,  "\\stoptext\n");
    safe_fclose(ARR);
    ConTeXtInterface = "en";
    RunConTeXtFile( ArrangeFile, "tex" );
  }
}

void RunSelect (char *FileName){
  char *SelectFile  = "texexec";
  FILE *SEL;
  fprintf(stdout, "               pdffile : %s\n",FileName);
  fprintf(stdout, "             backspace : %s\n",BackSpace);
  fprintf(stdout, "              topspace : %s\n",TopSpace);
  fprintf(stdout, "           paperoffset : %s\n",PaperOffset);
  if (STREQ(TextWidth,"0pt")) { fprintf(stdout, "             textwidth : unknown\n"); }
  else { fprintf(stdout, "             textwidth : %s\n",TextWidth); }
  char *texfile=NULL;
  my_concat(&texfile,SelectFile,".tex");
  safe_fopen( SEL, texfile, "w");
  if (SEL) {
    fprintf(SEL, "%% format=english\n");
    if (!STREQ(PaperFormat,"standard")) {
      char *zPaperFormat = safe_strdup(PaperFormat);
      char *runner = zPaperFormat;
      while (*runner) {
	if (*runner == 'x' || *runner == 'X') 
	  *runner = '*';
	runner++;
      }
      runner = zPaperFormat;
      char *from = next_word(&runner,"*");
      char *to = runner;
      if (!(to && strlen(to))) { to = from; }
      fprintf(SEL, "\\setuppapersize[%s][%s]\n", from,to);
    }
    fprintf( SEL, "\\definepapersize\n");
    fprintf( SEL, "  [offset=%s]\n",PaperOffset);
    fprintf( SEL, "\\setuplayout\n");
    fprintf( SEL, "  [backspace=%s,\n",BackSpace);
    fprintf( SEL, "    topspace=%s,\n",TopSpace);
    if (Markings) {
      fprintf( SEL, "     marking=on,\n");
      fprintf(stdout, "           cutmarkings : on\n");
    }
    fprintf(SEL,"       width=middle,\n");
    fprintf(SEL,"      height=middle,\n");
    fprintf(SEL,"    location=middle,\n");
    fprintf(SEL,"      header=0pt,\n");
    fprintf(SEL,"      footer=0pt]\n");
    fprintf(SEL,"\\setupexternalfigures\n");
    fprintf(SEL,"  [directory=]\n");
    fprintf(SEL,"\\starttext\n");
    if (strlen(Selection)) {
      fprintf(SEL,"\\filterpages\n");
      fprintf(SEL,"  [%s][%s][width=%s]\n",FileName,Selection,TextWidth);
    }
    fprintf(SEL,"\\stoptext\n");
    safe_fclose(SEL);
    ConTeXtInterface = "en";
    RunConTeXtFile( SelectFile, "tex" );
  }
}


void RunCopy (char *FileName){
  char *CopyFile    = "texexec";
  FILE *COP;
  fprintf(stdout, "               pdffile : %s\n",FileName);
  if (STREQ(PageScale,"1000")) {
    fprintf(stdout, "                offset : %s\n",PaperOffset);
  } else {
    fprintf(stdout, "                 scale : %s\n",PageScale);
    if (atoi(PageScale) < 10) { PageScale = Itoa((int)(atof(PageScale) * 1000)); }
  }
  char *texfile = NULL;
  my_concat(&texfile,CopyFile,".tex");
  safe_fopen( COP, texfile, "w");
  if (COP){
    fprintf(COP, "%% format=english\n");
    fprintf(COP, "\\getfiguredimensions\n");
    fprintf(COP, "  [%s][page=1]\n",FileName);
    fprintf(COP, "\\definepapersize\n");
    fprintf(COP, "  [copy]\n");
    fprintf(COP, "  [width=\\naturalfigurewidth,\n");
    fprintf(COP, "   height=\\naturalfigureheight]\n");
    fprintf(COP, "\\setuppapersize\n");
    fprintf(COP, "  [copy][copy]\n");
    fprintf(COP, "\\setuplayout\n");
    fprintf(COP, "  [location=middle,\n");
    fprintf(COP, "   topspace=0pt,\n");
    fprintf(COP, "   backspace=0pt,\n");
    fprintf(COP, "   header=0pt,\n");
    fprintf(COP, "   footer=0pt,\n");
    fprintf(COP, "   width=middle,\n");
    fprintf(COP, "   height=middle]\n");
    fprintf(COP, "\\setupexternalfigures\n");
    fprintf(COP, "  [directory=]\n");
    fprintf(COP, "\\starttext\n");
    fprintf(COP, "\\copypages\n");
    fprintf(COP, "  [%s]\n",FileName);
    fprintf(COP, "  [scale=%d,\n",(int)PageScale);
    if (Markings) {
      fprintf(COP, "   marking=on,\n");
      fprintf(stdout, "           cutmarkings : on\n");
    }
    fprintf(COP, "   offset=%s]\n",PaperOffset);
    fprintf(COP, "\\stoptext\n");
    safe_fclose(COP);
    ConTeXtInterface = "en";
    RunConTeXtFile( CopyFile, "tex" );
  }
}

void RunCombine (int Filecount, string *Files){
  char *CombineFile = "texexec";
  char *zCombination = safe_strdup(Combination);
  char *runner = zCombination;
  while (*runner) {
    if (*runner == 'x' || *runner == 'X') 
      *runner = '*';
    runner++;
  }
  runner = zCombination;
  char *nx = next_word(&runner,"*");
  char *ny = runner;
  if (!(nx && ny && strlen(nx) && strlen(ny)))    
    return;
  fprintf(stdout,"           combination : %s\n",zCombination);
  FILE *COM;
  char *texfile=NULL;
  my_concat(&texfile,CombineFile,".tex");
  safe_fopen( COM, texfile,"w");
  if(COM) {
    fprintf(COM, "%% format=english\n");
    if (!STREQ(PaperFormat,"standard")) {
      char *zPaperFormat = safe_strdup(PaperFormat);
      runner = zPaperFormat;
      while (*runner) {
	if (*runner == 'x' || *runner == 'X') 
	  *runner = '*';
	runner++;
      }
      runner = zPaperFormat;
      char *from = next_word(&runner,"*");
      char *to = runner;
      if (!(to && strlen(to))) { to = from; }
      fprintf(stdout,"             papersize : %s\n",PaperFormat);
      fprintf(COM, "\\setuppapersize[%s][%s]\n", from,to);
    }
    if (STREQ(PaperOffset,"0pt")) { PaperOffset = "1cm"; }
    fprintf(stdout, "          paper offset : %s\n",PaperOffset);
    fprintf(COM, "\\setuplayout\n");
    fprintf(COM, "  [topspace=%s,\n",PaperOffset);
    fprintf(COM, "   backspace=%s,\n",PaperOffset);
    fprintf(COM, "   header=0pt,\n");
    fprintf(COM, "   footer=1cm,\n");
    fprintf(COM, "   width=middle,\n");
    fprintf(COM, "   height=middle]\n");
    if (NoBanner) {
      fprintf(COM, "\\setuplayout\n");
      fprintf(COM, "  [footer=0cm]\n");
    }
    fprintf(COM, "\\setupexternalfigures\n");
    fprintf(COM, "  [directory=]\n");
    fprintf(COM, "\\starttext\n");
    int k;
    for (k=0;k<Filecount;k++) {
      char *FileName = Files[k];
      if (strncasecmp(FileName,"texexec",7))
	continue;
      if (strlen(Result) && strncasecmp(FileName,Result,strlen(Result)))
	continue;
      fprintf(stdout, "               pdffile : %s\n",FileName);
      char *CleanFileName = CleanTeXFileName(FileName);
      fprintf(COM, "\\setupfootertexts\n");
      fprintf(COM, "  [%s\\space---\\space\\currentdate\\space---\\space\\pagenumber]\n",CleanFileName);
      fprintf(COM, "\\combinepages[%s][nx=%s,ny=%s]\n",FileName,nx,ny);
      fprintf(COM, "\\page\n");
    }
    fprintf(COM, "\\stoptext\n");
    safe_fclose(COM);
    ConTeXtInterface = "en";
    RunConTeXtFile( CombineFile, "tex" );
  }
}


char *LocatedFormatPath (char *FormatPath) {
  if (STREQ(FormatPath,""))  {
    if ((FormatPath = discover_path("fmt"))) {
      //      FormatPath = chomp(FormatPath);
      char *runner = strstr(FormatPath,".;");
      runner += 2;
      FormatPath = runner;
      runner = strchr(FormatPath,';');
      *runner = 0;
      FormatPath = unix_path(FormatPath);
      char *newpath = safe_malloc(strlen(FormatPath));
      runner = FormatPath;
      int k=0;
      while (*runner) {
	if (*runner == '!') {
	} else if (*runner=='/' && *(runner+1) =='/') {
	  newpath[k++] = *runner;
	  runner++;
	} else {
	  newpath[k++] = *runner;
	}
	runner++;
      }
      *runner =  0;
      safe_free(FormatPath);
      FormatPath = newpath;
      if ((strlen(FormatPath)) && Verbose ) {
	fprintf(stdout, "    located formatpath : %s\n",FormatPath);
      }
    }
  }
  return FormatPath;
}

void RunOneFormat (char *FormatName,  char *fmtutil) {
  char *TeXFormatPath = "";
  char *TeXPrefix = "";
  if (strlen(fmtutil) && (!(strcasestr(FormatName,"metafun") || 
			    strcasestr(FormatName,"mptopdf")))) {
    char *cmd=NULL;
    my_concat3(&cmd,fmtutil," --byfmt  ",FormatName);
    if (Verbose) { fprintf(stdout, "\n%s\n\n",cmd); }
    MakeUserFile();   // this works only when the path is kept
    MakeResponseFile();
    Problems = system(cmd);
    RemoveResponseFile();
    RestoreUserFile();
  } else {
    Problems = 1;
  }
  if (Problems) {
    Problems = 0;
    if (strcasestr(TeXExecutable,"etex") ||
	strcasestr(TeXExecutable,"eetex") ||
	strcasestr(TeXExecutable,"pdfetex") ||
	strcasestr(TeXExecutable,"pdfeetex") ||
	strcasestr(TeXExecutable,"eomega")) {
      TeXPrefix = "*";
    }
    char CurrentPath[256];
    getcwd(CurrentPath,256);
    TeXFormatPath = LocatedFormatPath(TeXFormatPath);
    if (strlen(TeXFormatPath)) { chdir (TeXFormatPath); }
    MakeUserFile();
    MakeResponseFile();
    char *exe = NULL;
    my_concat(&exe,TeXProgramPath,TeXExecutable);
    if ((own_path[0]!= '"') && (strchr(TeXProgramPath,' ')!=NULL))
      my_concat3(&exe,"\"",exe,"\"");
    char *cmd = NULL;
    my_concat3(&cmd,exe," ",TeXVirginFlag); 
    my_concat3(&cmd,cmd," ",TeXPassString);
    my_concat3(&cmd,cmd," ",PassOn);
    my_concat3(&cmd,cmd," ",TeXPrefix);
    my_concat(&cmd,cmd,FormatName);
    if (Verbose) { fprintf(stdout, "\n%s\n\n",cmd); }
    Problems = system(cmd) ;
    RemoveResponseFile();
    RestoreUserFile();
    if ((strlen(TeXFormatPath)) && (strlen(CurrentPath))) {
      chdir (CurrentPath);
    }
  }
}

void RunMpFormat (char *MpFormat) {
  if (!strlen(MpFormat))
    return;
  char CurrentPath[256];
  getcwd(CurrentPath,256);
  MpFormatPath = LocatedFormatPath(MpFormatPath);
  if (strlen(MpFormatPath)) { 
    chdir (MpFormatPath);
  }
  char *exe = MpExecutable;
  if ((own_path[0]!= '"') && (strchr(MpExecutable,' ')!=NULL))
    my_concat3(&exe,"\"",exe,"\"");
  char *cmd = NULL;
  my_concat3(&cmd,exe," ",MpVirginFlag); 
  my_concat3(&cmd,cmd," ",MpPassString);
  my_concat3(&cmd,cmd," ",MpFormat);
  if (Verbose) { fprintf(stdout, "\n%s\n\n",cmd); }
  system(cmd ) ;
  if ( (strlen(MpFormatPath)) && ( strlen(CurrentPath)) ) {
    chdir (CurrentPath);
  }
}



void RunFormats (int ARGC, string *ARGV, char *fmtutil){
  char *ConTeXtFormatsPrefix;
  int MetaFunDone = 0;
  string ConTeXtFormatsS[] = { "nl", "en", "de", "cz", "uk", "it", "ro", "xx" };
  int ConTeXtFormatsSSize = 8;
  string *ConTeXtFormats;
  int ConTeXtFormatSize;
  if (ARGC) { ConTeXtFormats = ARGV; ConTeXtFormatSize = ARGC;}
  else if (strlen(UsedInterfaces)) {
    ConTeXtFormats = malloc(sizeof(char *)*((strlen(UsedInterfaces)/2)+1));
    int k=0;
    char *f;
    while ((f = words(&UsedInterfaces,", "))) {
      ConTeXtFormats[k++] = f;
    }
    ConTeXtFormatSize = (k-1);
  } else {
    ConTeXtFormats = ConTeXtFormatsS;
    ConTeXtFormatSize =  ConTeXtFormatsSSize;
  }
  if (strlen(Format)) { 
    ConTeXtFormats = malloc(sizeof(char *));
    ConTeXtFormatSize = 1;
    ConTeXtFormats[0] = Format; 
    ConTeXtFormatsPrefix = ""; 
  } else { 
    ConTeXtFormatsPrefix = "cont-"; 
  }
  if (strlen(TeXHashExecutable)) {
    fprintf(stdout, "\n");
    fprintf(stdout, "       TeX hash binary : %s%s\n",TeXProgramPath,TeXHashExecutable);
    fprintf(stdout, "               comment : hashing may take a while ...\n");
    char *exe = NULL;
    my_concat(&exe,TeXProgramPath,TeXHashExecutable);
    if ((own_path[0]!= '"') && (strchr(TeXProgramPath,' ')!=NULL))
      my_concat3(&exe,"\"",exe,"\"");
    if (Verbose) { fprintf(stdout, "\n%s\n\n",exe); }
    system(exe);
  }
  int i;
  char *theformat = NULL;
  for (i=0;i<ConTeXtFormatSize;i++) {
    char *Interface =ConTeXtFormats[i];
    if (STREQ(Interface,MetaFun)) {
      RunMpFormat(MetaFun);
      MetaFunDone = 1;
    } else if (STREQ(Interface,MpToPdf)) {
      if (strcasestr(TeXExecutable,"pdf")!=NULL) { RunOneFormat(MpToPdf,fmtutil); }
    } else {
      my_concat(&theformat,ConTeXtFormatsPrefix,Interface);
      RunOneFormat(theformat,fmtutil);
    }
  }
  fprintf(stdout, "\n");
  fprintf(stdout, "            TeX binary : %s%s\n",TeXProgramPath,TeXExecutable);
  fprintf(stdout, "             format(s) : ");
  for (i=0;i<ConTeXtFormatSize;i++) {
    char *Interface =ConTeXtFormats[i];
    fprintf(stdout,"%s",Interface);
    if (i)
      fprintf(stdout," ");
  }
  fprintf(stdout, "\n\n");
}




void RunFiles (int ARGC,string *ARGV) {
  char *pdffile = NULL;
  if (PdfArrange) {
    string *arrangedfiles=malloc(sizeof(char*)*ARGC);
    int arrangedfilessize=0;
    int i=0;
    for (i=0;i<ARGC;i++) {
      char *JobName = ARGV[i];
      if (!(strcasestr(JobName,".pdf")==(JobName+strlen(JobName)-4))) {
	my_concat(&pdffile,JobName,".pdf");
	if (file_exists(pdffile)) { JobName= pdffile; }
	else { my_concat(&JobName,JobName,".PDF"); }
      }
      arrangedfiles[arrangedfilessize++] = JobName;
    }
    if (arrangedfilessize) { 
      RunArrange(arrangedfilessize,arrangedfiles) ;
    }
  } else if ( (PdfSelect) || (PdfCopy) || (PdfCombine) ) {
    char *JobName = ARGV[0];
    if (strlen(JobName)) {
      if (!(strcasestr(JobName,".pdf")==(JobName+strlen(JobName)-4))) {
	my_concat(&pdffile,JobName,".pdf");
	if (file_exists(pdffile)) { JobName= pdffile; }
	else { my_concat(&JobName,JobName,".PDF"); }
      }
      if      (PdfSelect) { RunSelect(JobName); }
      else if (PdfCopy)   { RunCopy(JobName); }
      else {
	RunCombine(ARGC,ARGV);
      }
    }
  } else if (TypesetModule) {
    RunModule(ARGC,ARGV);
  } else {
    char *JobSuffix = "tex";
    int i=0;
    for (i=0;i<ARGC;i++) {
      char *JobName = safe_strdup(ARGV[i]);
      if (strrchr(JobName,'.')!=NULL && 
	  strrchr(JobName,'.')<(JobName+strlen(JobName))) {
	JobSuffix = strrchr(JobName,'.');
	*JobSuffix = 0;
	JobSuffix++;
      }
      if ((!(strlen(Format)) || (strcasestr(Format,"cont")==Format))) {
	RunConTeXtFile( JobName, JobSuffix );
      } else {
	RunSomeTeXFile( JobName, JobSuffix );
      }
      char *file = NULL;
      my_concat(&file,JobName,".log");
      if (file_exists(file) && file_size(file)==0)
	unlink(file);
      my_concat(&file,JobName,".tui");
      if (file_exists(file) && file_size(file)==0)
	unlink(file);
    }
  }
}


int checkMPgraphics (char *MpName) {
  unsigned mpochecksum = 0;
  char *cmdline = MpName;
  if (strlen(MakeMpy)) { 
    my_concat3(&cmdline,cmdline," --",MakeMpy);
  }
  if (MpyForce)        { 
    my_concat(&cmdline,cmdline," --force");
  }  else {
    char *file = NULL;
    my_concat(&file,MpName,".mpo") ;
    if (!(file_exists(file) && (file_size(file)>32)))
      return 0;
    FILE *MPO;
    safe_fopen(MPO,file,"r");
    if (!MPO)
      return 0;
    char *buf = safe_malloc(file_size(file)+1);
    if(fread(&buf,1,file_size(file),MPO)) {
      mpochecksum = (cksum(buf) % 65535);
      safe_free(buf);
    }
    safe_fclose(MPO);
    FILE *MPY;
    file = NULL;
    my_concat(&file,MpName,".mpy") ;
    safe_fopen(MPY,file,"r");
    if (MPY) {
      readline(MPY,&buf);
      char *str = chomp (buf);
      safe_fclose(MPY);
      if ((strlen(str)>14) && (str[0] == '%')) {
	str++;
	while (isspace(*str)) str++;
	if (strstr(str,"mpochecksum")==str) {
	  str+=11;
	  while (isspace(*str)) str++;
	  if (*str == ':') str++;
	  while (isspace(*str)) str++;
	  unsigned test = (unsigned)atol(str);
	  if ( ( mpochecksum == test ) && ( mpochecksum != 0 ) )
	    return 0 ;
	}
      }
    }
  }
  RunPerlScript( "makempy", cmdline);
  fprintf(stdout, "  second MP run needed : text graphics found\n");
  return 1;
}

int checkMPlabels (char *MpName, Hash mpbetex) {
  char *file = MpName;
  if (!(file_exists(file) && (file_size(file)>10)))
    return 0;
  FILE *MP;
  safe_fopen(MP,file,"r");
  if (!MP)
    return 0;
  int n = 0;
  char *line = malloc(sizeof(char *)*BUFSIZE);
  while (readline(MP,&line)) {
    if ((strlen(line)>13) && strstr(line,"% figure ")==line) {
      line +=9;
      int num = 0;
      while (isdigit(*line)) {
	num = num * 10 + *line;
	line++;
      }
      if (isspace(*line)) line++;
      if (*line == ':') line++;
      if (isspace(*line)) line++;
      if (get_item(mpbetex,Itoa(num))){
	char *back = NULL;
	my_concat (&back,hash_value(mpbetex,Itoa(num)),line);
	set_value(mpbetex,Itoa(num),back);
      } else {
	set_value(mpbetex,Itoa(num),safe_strdup(line));
      }
      ++n ;
    }
  }
  safe_fclose(MP);
  if (n)
    fprintf(stdout, "  second MP run needed : %d tex labels found\n",n);
  return n;
 }

void doRunMP (char *MpName, int MergeBE, Hash mpbetex){
    int TexFound = 0;
    char *MpTmp = "tmpgraph";  
    char *MpKep = "tmpgraph.kep";
    //    char *MpLog = "tmpgraph.log";
    char *MpBck = "tmpgraph.bck";
    char *MpTex = "tmpgraph.tex";
    char *MpDvi = "tmpgraph.dvi";
    char *MpFile   = NULL;
    my_concat(&MpFile,MpName,".mp");
    FILE *MP;
    safe_fopen(MP,MpFile,"r");
    if (MP) {
      char *MPdata=NULL;
      char *line = malloc(sizeof(char *)*BUFSIZE);
      while (readline(MP,&line)) {
	if ((strlen(line) && line[0] == '%')) 
	  my_concat(&MPdata,MPdata,line);
      }
      safe_fclose(MP);
      safe_free(line);
      unlink(MpKep);
      if (file_exists(MpKep))
	return;
      rename( MpFile, MpKep);
      TexFound = (MergeBE || (strstr(MPdata,"btex ") && strstr(MPdata," etex") &&
			      strstr(MPdata,"btex ") < strstr(MPdata," etex")));
      if (!file_exists(MpFile)) {
	safe_fopen(MP,MpFile,"w");
	int inlabel = 0;
	char *Newdata=malloc(strlen(MPdata)*2);
	int k;
	int end =strlen(MPdata);
	int n = 0;
	for (k=0;k<end;k++) {
	  char c = MPdata[k];
          if (c == 'b') {
	    if (!inlabel &&  k < (end-4) && 
		MPdata[(k+1)]=='t' && MPdata[(k+2)]=='e' && MPdata[(k+3)]=='x') {
	      inlabel=1;
	    }
	    Newdata[n++] = c;
	  } else if (c == 'e') {
	    if (inlabel &&  k < (end-4) &&
		MPdata[(k+1)]=='t' && MPdata[(k+2)]=='e' && MPdata[(k+3)]=='x') {
	      inlabel = 0;
	    }
	    Newdata[n++] = c;
	  } else if (c == ';') { 
	    Newdata[n++] = c;
	    if (!inlabel) {
	      Newdata[n++] = '\n';
	    }
	  } else if (c == '\n') {
	    while (MPdata[(k+1)] && MPdata[(k+1)] == '\n') k++;
	    Newdata[n++] = '\n';
	  } else {
	    Newdata[n++] = c;
	  }
	}
	Newdata[n] = 0;
	safe_free(MPdata);
	MPdata = safe_strdup(Newdata);
	safe_free(Newdata);
	int zeroseen = 0;
	if (MergeBE) {
	  char *next;
	  while ((next=strcasestr(MPdata,"beginfig"))!=NULL) {
	    char *seen = strndup(MPdata,(next-MPdata));
	    my_concat(&Newdata,Newdata,seen);
	    MPdata = next;
	    next+=9;
	    while (isspace(*next)) next++;
	    if (*next == '(') {
	      next++;
	      while (isspace(*next)) next++;
	      int val=0;
	      while (isdigit(*next)) {
		val = val*10+isdigit(*next);
		next++;
	      }
	      while (isspace(*next)) next++;
	      if (*next == ')') {
		while (isspace(*next)) next++;
		if (*next == ';') {
		  char *addf = NULL;
		  if (val == 0)
		    zeroseen = 1;
		  my_concat3(&addf,"beginfig(",Itoa(val),");\n");
		  my_concat3(&addf,addf,hash_value(mpbetex,Itoa(val)),"\n");
		  safe_free(addf);
		  my_concat(&Newdata,Newdata,addf);
		} else {
		  seen = strndup(MPdata,(next-MPdata));
		  my_concat(&Newdata,Newdata,seen);
		  MPdata = next;
		}
	      } else {
		seen = strndup(MPdata,(next-MPdata));
		my_concat(&Newdata,Newdata,seen);
		MPdata = next;
	      }
	    } else {
	      seen = strndup(MPdata,(next-MPdata));
	      my_concat(&Newdata,Newdata,seen);
	      MPdata = next;
	    }
	  }
	  my_concat(&Newdata,Newdata,MPdata); /* the tail */
	}
	if (!zeroseen) { 
	  fprintf(MP,"%s",hash_value(mpbetex,"0"));
	}
	fprintf(MP,"%s",MPdata);
	fprintf(MP, "\nend\n");
	safe_fclose(MP);
      }
      if (TexFound) {
	fprintf(stdout, "       metapost to tex : %s\n",MpName);
	char *own_quote = "";
	if (MpToTeXExecutable[0] != '"' && (strchr(MpToTeXExecutable,' ')!=NULL))
	  own_quote= "\"";
	char *exe= safe_strdup(MpToTeXExecutable);
	my_concat3(&exe,own_quote,exe,own_quote);
	my_concat3(&exe,exe," ",MpFile);
	my_concat3(&exe,exe," > ",MpTex);
	Problems = system(exe);
	if (file_exists(MpTex) && !Problems ) {
	  FILE *TMP;
	  safe_fopen(TMP, MpTex,"a");
	  if (TMP) {
	    fprintf(TMP, "\\end{document}\n");
	    safe_fclose(TMP);
	    if ((!strlen(Format)) || (strcasestr(Format,"cont")==Format) ) {
	      OutputFormat = "dvips";
	      RunConTeXtFile(MpTmp, "tex" );
	    } else {
	      RunSomeTeXFile(MpTmp, "tex" );
	    }
	    if (file_exists(MpDvi) && !Problems ) {
	      fprintf(stdout, "       dvi to metapost : %s\n",MpName);
	      own_quote="";
	      if (DviToMpExecutable[0] != '"' && (strchr(DviToMpExecutable,' ')!=NULL))
		own_quote= "\"";
	      exe= safe_strdup(DviToMpExecutable);
	      my_concat3(&exe,own_quote,exe,own_quote);
	      my_concat3(&exe,exe," ",MpDvi);
	      my_concat3(&exe,exe," ",MpName);
	      my_concat(&exe,exe,".mpx");
	      Problems = system(exe);
	    }
	    unlink (MpBck);
	    rename (MpTex, MpBck);
	    unlink (MpDvi);
	  }
        }
        fprintf(stdout, "              metapost : %s\n",MpName);
	own_quote="";
	if (MpExecutable[0] != '"' && (strchr(MpExecutable,' ')!=NULL))
	  own_quote= "\"";
	exe= safe_strdup(MpExecutable);
	my_concat3(&exe,own_quote,exe,own_quote);
	if (EnterMode==Batch)     { my_concat3(&exe,exe," ",MpBatchFlag); }
	if (EnterMode==NonStop)   { my_concat3(&exe,exe," ",MpNonStopFlag); }
        if (strlen(MpFormat) && ( strcasestr(MpFormat,"plain")==NULL &&
				  strcasestr(MpFormat,"mpost")==NULL )) {
	  fprintf(stdout,"                format : %s\n",MpFormat);
	  my_concat3(&exe,exe," ",MpPassString);
	  my_concat3(&exe,exe," ",MpFormatFlag);
	  my_concat(&exe,exe,MpFormat);
        }
	my_concat3(&exe,exe," ",MpName);
        Problems = system(exe);
	FILE *MPL;
	char *file = NULL;
	my_concat(&file,MpName,".log");
	safe_fopen( MPL, file, "r" );
	if (MPL){
	  char *line = malloc(sizeof(char *)*BUFSIZE);
	  while (readline(MPL,&line)) {
	    int val;
	    char *next;
	    if (strstr(line,"l.")==line) {
	      next = line+2;
	      val = 0;
	      while (isdigit(*next)) {
		val = val*10+isdigit(*next);
		next++;
	      }
	      if (isspace(*next)) {
                fprintf(stdout, " error in metapost run : %s.mp:%d\n",MpName,val);
	      }
	    }
	  }
	}
	unlink ("mptrace.tmp");
        rename( MpFile, "mptrace.tmp" );
        if (file_exists(MpKep)) {
            unlink(MpFile);
            rename(MpKep, MpFile);
        }
      }
    }
}

void RunMP (int ARGC, string *ARGV) {
  if ((MpExecutable) && (MpToTeXExecutable) && (DviToMpExecutable) ) {
    int i;
    for (i=0;i<ARGC;i++) {
      char *RawMpName = safe_strdup(ARGV[i]);
      char *MpName;
      char *Rest="";
      if (strchr(RawMpName,'.')) {
	MpName = next_word(&RawMpName,".");
	Rest = RawMpName;
      } else {
	MpName = RawMpName;
      }
      char *MpFile = NULL;
      my_concat(&MpFile,MpName,".mp");
      if (file_exists(MpFile) && file_size(MpFile)>25) {
	char *file = NULL;
	my_concat(&file,MpName,".mpt");
	unlink (file);
	Hash mpbetex = make_hash();
	doRunMP( MpName, 0 , mpbetex);
	int mpgraphics = checkMPgraphics(MpName);
	int mplabels = checkMPlabels(MpName, mpbetex);
	if ( mpgraphics || mplabels ) {
	  doRunMP (MpName, mplabels, mpbetex );
	}
      }
    }
  }
}

void RunMPX (char *MpName){
  char *MpTex = "tmpgraph.tex";
  char *MpDvi = "tmpgraph.dvi";
  char *MpTmp = "tmpgraph";  
  *(strrchr(MpName,'.')) = 0;
  char *MpFile = NULL;
  my_concat(&MpFile,MpName,".mp");
  FILE *MP;
  size_t filesize;
  if ((MpToTeXExecutable) 
      && (DviToMpExecutable)
      && (file_exists(MpFile)) 
      && ((filesize = file_size(MpFile)) > 5 )
      && (safe_fopen(MP, MpFile, "r")) 
      && MP ) {
    char *mpdata = malloc(filesize+1);
    fread(mpdata,1,filesize,MP);
    safe_fclose(MP);
    if (strstr(mpdata,"btex")!=NULL ||
	strstr(mpdata,"etex")!=NULL ||
	strstr(mpdata,"verbatimtex")!=NULL) {
      fprintf(stdout,"   generating mpx file : %s\n",MpName);
      char *own_quote = "";
      if (MpToTeXExecutable[0] != '"' && (strchr(MpToTeXExecutable,' ')!=NULL))
	  own_quote= "\"";
      char *exe= safe_strdup(MpToTeXExecutable);
      my_concat3(&exe,own_quote,exe,own_quote);
      my_concat3(&exe,exe," ",MpFile);
      my_concat3(&exe,exe," > ",MpTex);
      Problems = system(exe);
      if (file_exists(MpTex) && !Problems ) {
	FILE *TMP;
	safe_fopen(TMP, MpTex,"a");
	if (TMP) {
	  fprintf(TMP, "\\end\n");
	  safe_fclose(TMP);
	  if ( (!strlen(Format)) || (strcasestr(Format,"cont")==Format)) {
	    RunConTeXtFile( MpTmp, "tex" );
	  } else {
	    RunSomeTeXFile( MpTmp, "tex" );
	  }
	  if (file_exists(MpDvi) && !Problems ) {
	    own_quote="";
	    if (DviToMpExecutable[0] != '"' && (strchr(DviToMpExecutable,' ')!=NULL))
	      own_quote= "\"";
	    exe= safe_strdup(DviToMpExecutable);
	    my_concat3(&exe,own_quote,exe,own_quote);
	    my_concat3(&exe,exe," ",MpDvi);
	    my_concat3(&exe,exe," ",MpName);
	    my_concat(&exe,exe,".mpx");
	    Problems = system(exe);
	  }
	  unlink (MpTex);
	  unlink (MpDvi);
	}
      }
    }
  }
}


void load_set_file (char *file, int trace) {
  Hash new = make_hash();
  Hash old = make_hash();
  FILE *BAT;
  safe_fopen(BAT,file,"r");
  if (BAT) {
    char *line = malloc(sizeof(char *)*BUFSIZE);
    while (readline(BAT,&line)) {
      line = chomp (line);
      char *next;
      char *var;
      char *val;
      if ((next = strcasestr(line,"SET"))) {
	next+=3;
	while (isspace(*next)) next++;
	if((var = next_word(&next,"="))) {
	  val = next;
	  trim(val);
	  if ((strlen(val)>2) && (val[0]=='%') && (strrchr(val,'%')==val+strlen(val))) {
	    val++;
	    val = chop (val);
	    val = getenv(val);
	  }
	  if (!get_item(old,var)) {
	    if (getenv(var)!=NULL) { 
	      set_value(old,var,safe_strdup(getenv(var)));
	    } else { 
	    set_value(old,var,safe_strdup(""));
	    }
	  }
	  set_value(old,var,safe_strdup(val));
	  setenv(var,val,1);
	}
      }
    }
    safe_fclose(BAT);
  }
  if (trace) {
    string *thekeys;
    int numkeys = keys(new,&thekeys);
    qsort(thekeys,numkeys,sizeof(char *),&my_strcmp);
    int k;
    for (k=0;k<numkeys;k++) {
      char *key = thekeys[k];
      if (!STREQ(hash_value(old,key),hash_value(new,key))) {
	fprintf(stdout, " changing env variable : '%s' from '%s' to '%s'\n",
		key,hash_value(old,key),hash_value(new,key));
      } else if (STREQ(hash_value(old,key),"")) {
	fprintf(stdout, "  setting env variable : '%s' to '%s'\n",key,hash_value(new,key));
      } else {
	fprintf(stdout, "  keeping env variable : '%s' at '%s'\n",key,hash_value(new,key));
      }
    }
    fprintf(stdout, "\n");
  }
}


int texexec_main(int ac,  string *av) {
  int optind;
  time_t TotalTime;
  struct tm *timeparts;
  int sec;
  int min;
  int escapeshell=0;
  STDOUT = stdout;
  STDERR = stderr;
  discover_init("texexec");
  Help = make_hash();
  ConTeXtInterfaces = make_hash();
  ResponseInterface = make_hash();
  OutputFormats = make_hash();
  read_all_options(); 
  setenv("MPXCOMMAND","0",1);
  if(getenv("SHELL") && ((strstr(getenv("SHELL"),"sh")) != NULL ||
			 (strstr(getenv("SHELL"),"SH")) != NULL))
	escapeshell = 1;
  TotalTime = time(NULL);
  timeparts = gmtime(&TotalTime);
  sec = timeparts->tm_sec;
  min = timeparts->tm_min;
  RandomSeed = (min*60+sec) % 1440;
  optind = parse_options (ac,av);
  int ARGC = (ac-optind);
  string *ARGV = av+optind;
  if (strlen(SetFile)) {
	load_set_file( SetFile, Verbose ); 
    SetFile = "" ;
  }
  signal(SIGINT,sig_ignore);
  /* various options */
  if (ARGC && ENDS_WITH(ARGV[0],".mpx")) {
    TeXProgram = "";
    DoMPXTeX   = 1;
    NoMPMode   = 1;
  }
  if (ScreenSaver) {
    ForceFullScreen = 1;
    TypesetFigures  = 'c';
    ProducePdfT     = 1;
    Purge           = 1;
  }
  if (DoMPTeX || DoMPXTeX) {
    RunOnce     = 1;
    ProducePdfT = 0;
    ProducePdfX = 0;
    ProducePdfM = 0;
  }
  if ( PdfArrange || PdfSelect || PdfCopy || PdfCombine ) {
    ProducePdfT = 1;
    RunOnce     = 1;
  }
  if      (ProducePdfT) { OutputFormat = "pdftex"; }
  else if (ProducePdfM) { OutputFormat = "dvipdfm"; }
  else if (ProducePdfX) { OutputFormat = "dvipdfmx"; }
  if (AutoPdf) {
    PdfOpen = PdfClose = 1 ;
  }
  if (RunOnce || strlen(Pages) || TypesetFigures || TypesetListing ) {  NOfRuns = 1; }
  if ((strlen(LogFile)) && ENDS_WITH(LogFile,".log")) {
    FILE *LOGFILE ;
    safe_fopen(LOGFILE, LogFile, "w" );
    STDOUT = LOGFILE;
    STDERR = LOGFILE;
  }
  fprintf(stdout,"\n%s\n\n", Program);
  char fullpath[256];
  getcwd(fullpath,256);
  if (Verbose) 
	fprintf(stdout,"          current path : %s\n", fullpath);
  char cur_path[3];
  cur_path[0] = '.'; cur_path[1] = pathslash; cur_path[2] = 0;
  char *own_file = NULL;
  if(getenv("FORGEDIR")) {
	own_path = strdup(getenv("FORGEDIR"));
	my_concat3(&own_file,own_path,pathslash_string,av[0]);
  } else {
#ifdef WIN32
	own_file = malloc(BUFSIZE);
	if(!GetModuleFileName(NULL,(LPTSTR)own_file,BUFSIZE)) { // oid
	  perror(NULL);
	  own_file = av[0];
	}
#else
	own_file = malloc(BUFSIZE);
	int chars;
	chars =readlink("/proc/self/exe",own_file,(BUFSIZE-1));
	if(chars> 0) {
	  own_file[chars+1]  = 0;
	} else {
	  perror(NULL);
	  own_file = av[0];
	}
#endif
	own_path = strdup(own_file);
	char *temp;
	if((temp = strrchr(own_path,pathslash)) == NULL) {
	  own_path = "";
	} else {
	  *(temp+1) = 0;
	}
  }
  // C version can shortcut some of this own_XX
  char *own_quote = "";
  if ((own_path[0]!='"') && strchr(own_path,' '))
	own_quote = "\"";
  string *paths = make_paths(getenv("PATH"));
  char *IniPath="";
  char *found_kpsewhich = "";
  if (!strlen(IniPath)) {
#ifndef USE_KPSE
	int k=0;
	char *pp;
	while ((pp = paths[k++]) != NULL) {
	  char *p=NULL;
	  my_concat(&p,checked_path(strdup(pp)),kpsewhich);
	  if (file_exists(p)) {
	    found_kpsewhich = p;
	    IniPath   = found_ini_file("ini");
	    if (!file_exists(IniPath))
	      IniPath = found_ini_file("rme") ;
	    break;
	  }
	}
#else
	IniPath   = found_ini_file("ini");
	if (!file_exists(IniPath))
	  IniPath = found_ini_file("rme") ;
	if (!IniPath)
	  IniPath = safe_strdup("");
#endif
    if (Verbose) {
      if (!strlen(found_kpsewhich)) {
#ifndef USE_KPSE
	fprintf(stdout, "     locating ini file : kpsewhich not found in path\n");
#else
	fprintf(stdout, "     locating ini file : using linked kpathsea library\n");
#endif
      } else {
	safe_free(found_kpsewhich);
      }
      if (!strlen(IniPath)) {
	fprintf(stdout,"     locating ini file : not found by kpsewhich\n");
      } else if ((strlen(IniPath)>3) && strcasecmp(IniPath+strlen(IniPath)-3,"rme")==0) {
	fprintf(stdout,"     locating ini file : not found by kpsewhich, using '.rme' file\n");
      } else {
	fprintf(stdout,"     locating ini file : found by kpsewhich\n");
      }
    }
  }
  struct locatedfile foundfile;
  if (!strlen(IniPath)) {
	foundfile = LocatedFile(own_path,"texexec.ini",cur_path);
	LocatedPath = foundfile.path;
	LocatedName = foundfile.name;
	IniPath     = foundfile.file;
    if (Verbose) {
	  if (!strlen(IniPath)) {
		fprintf(stdout,"     locating ini file : not found by searching\n");
	  } else {
		fprintf(stdout,"     locating ini file : found by searching\n");
	  }
    }
  }
  if (!strlen(IniPath)) {
    if (getenv("TEXEXEC_INI_FILE") ) {
      my_concat(&IniPath,checked_path(getenv("TEXEXEC_INI_FILE")),"texexec.ini");
      if(!file_exists(IniPath)) { IniPath = strdup(""); }
    }
    if (Verbose) {
      if (!strlen(IniPath)) {
	fprintf(stdout,"     locating ini file : no environment variable set\n");
      } else {
	fprintf(stdout,"     locating ini file : found by environment variable\n");
      }
    }
  }
  if (!strlen(IniPath)) {
    my_concat(&IniPath,own_path,"texexec.ini");
    if(!file_exists(IniPath)) { IniPath = strdup(""); }
    if (Verbose) {
      if (!strlen(IniPath)) {
	fprintf(stdout, "     locating ini file : not found in own path\n");
      } else {
	fprintf(stdout, "     locating ini file : found in own path\n");
      }
    }
  }
  if (!strlen(IniPath)) { IniPath = "texexec.ini"; }
 
  Hash Done = make_hash();
  read_ini_file(Done,IniPath,Verbose);
  TeXShell          = IniValue(Done, "TeXShell",          "" );
  SetupPath         = IniValue(Done, "SetupPath",         "" );
  UserInterface     = IniValue(Done, "UserInterface",     "en" );
  UsedInterfaces    = IniValue(Done, "UsedInterfaces",    "en" );
  TeXFontsPath      = IniValue(Done, "TeXFontsPath",      "." );
  MpExecutable      = IniValue(Done, "MpExecutable",      "mpost" );
  MpToTeXExecutable = IniValue(Done, "MpToTeXExecutable", "mpto" );
  DviToMpExecutable = IniValue(Done, "DviToMpExecutable", "dvitomp" );
  TeXProgramPath    = IniValue(Done, "TeXProgramPath",    "" );
  TeXFormatPath     = IniValue(Done, "TeXFormatPath",     "" );
  ConTeXtPath       = IniValue(Done, "ConTeXtPath",       "" );
  TeXScriptsPath    = IniValue(Done, "TeXScriptsPath",    "" );
  TeXHashExecutable = IniValue(Done, "TeXHashExecutable", "" );
  TeXExecutable     = IniValue(Done, "TeXExecutable",     "tex" );
  TeXVirginFlag     = IniValue(Done, "TeXVirginFlag",     "-ini" );
  TeXBatchFlag      = IniValue(Done, "TeXBatchFlag",      "-int=batchmode" );
  TeXNonStopFlag    = IniValue(Done, "TeXNonStopFlag",    "-int=nonstopmode" );
  MpBatchFlag       = IniValue(Done, "MpBatchFlag",       "-int=batchmode" );
  MpNonStopFlag     = IniValue(Done, "MpNonStopFlag",     "-int=nonstopmode" );
  TeXPassString     = IniValue(Done, "TeXPassString",     "" );
  TeXFormatFlag     = IniValue(Done, "TeXFormatFlag",     "" );
  MpFormatFlag      = IniValue(Done, "MpFormatFlag",      "" );
  MpVirginFlag      = IniValue(Done, "MpVirginFlag",      "-ini" );
  MpPassString      = IniValue(Done, "MpPassString",      "" );
  MpFormat          = IniValue(Done, "MpFormat",          MetaFun );
  MpFormatPath      = IniValue(Done, "MpFormatPath",      TeXFormatPath );

  FmtLanguage       = IniValue(Done, "FmtLanguage", "" );
  FmtBodyFont       = IniValue(Done, "FmtBodyFont", "" );
  FmtResponse       = IniValue(Done, "FmtResponse", "" );
  TcXPath           = IniValue(Done, "TcXPath",     "" );
  SetFile           = IniValue(Done, "SetFile", SetFile);
  if (Verbose) {
    fprintf(stdout, "\n");
    char *CnfFile = discover_file("texmf.cnf","cnf");
    if(strlen(CnfFile)) {
      fprintf (stdout," applications will use : %s\n", CnfFile);
      safe_free(CnfFile);
    }
  }
  if (strlen(FmtLanguage) && STREQ(MainLanguage,"standard"))
    MainLanguage = FmtLanguage;
  if (strlen(FmtBodyFont) && STREQ(MainBodyFont,"standard"))
    MainBodyFont = FmtBodyFont;
  if (strlen(FmtResponse) && STREQ(MainResponse,"standard"))
    MainResponse = FmtResponse;
  if (!strlen(TeXFormatFlag)) { TeXFormatFlag = "&"; }
  if (!strlen(MpFormatFlag)) { MpFormatFlag = "&"; }
  if (!(dosish && !escapeshell )) {
    if (STREQ(TeXFormatFlag,"&")) { TeXFormatFlag = "\\&"; }
    if (STREQ(MpFormatFlag,"&"))  { MpFormatFlag  = "\\&"; }
  }
  if (strlen(TeXProgram)) { TeXExecutable = TeXProgram; }
  char *fmtutil= "";
  if (MakeFormats || Verbose) {
    if (Alone) {
      if (Verbose) { fprintf(stdout, "     generating format : not using fmtutil\n"); }
    } else if ((strcasestr(TeXShell,"tetex") != NULL) ||
	       (strcasestr(TeXShell,"fptex") != NULL)) {
      int k=0;
      char *pp;
      while ((pp = paths[k++]) != NULL) {
	char *p=NULL;
	char *p_extra=NULL;
	my_concat(&p,checked_path(strdup(pp)),"fmtutil");
	my_concat(&p_extra,checked_path(strdup(pp)),"fmtutil.exe");
	if (file_exists(p)) {
	  fmtutil = p;
	  break;
	}
	if (file_exists(p_extra)) {
	  fmtutil = p_extra;
	  break;
	}
      }
      if ((fmtutil[0]!= '"') && (strchr(fmtutil,' ')!=NULL))
	my_concat3(&fmtutil ,"\"", fmtutil, "\"");
      if (Verbose) {
	if (!strlen(fmtutil)) {
	  fprintf(stdout, "      locating fmtutil : not found in path\n");
	} else {
	  fprintf(stdout, "      locating fmtutil : %s\n",fmtutil);
	}
      }
    }
  }
  if (Verbose) { fprintf(stdout, "\n"); }
  if (!strlen(TeXScriptsPath)) { TeXScriptsPath = own_path; }
  if (!strlen(ConTeXtPath)) { ConTeXtPath = TeXScriptsPath; }
  if (getenv("HOME")) {
    if (SetupPath) { my_concat(&SetupPath,SetupPath,","); }
    my_concat(&SetupPath,SetupPath,getenv("HOME"));
  }
  if (strlen(TeXFormatPath))  { TeXFormatPath  = checked_path(TeXFormatPath); }
  if (strlen(MpFormatPath))   { MpFormatPath   = checked_path(MpFormatPath); }
  if (strlen(ConTeXtPath))    { ConTeXtPath    = checked_path(ConTeXtPath); }
  if (strlen(SetupPath))      { SetupPath      = checked_path(SetupPath);}
  if (strlen(TeXScriptsPath)) { TeXScriptsPath = checked_path(TeXScriptsPath); }
  SetupPath = QuotePath(SetupPath);
  SetupPath = checked_path(SetupPath);
  set_value(OutputFormats, "pdf","pdftex");
  set_value(OutputFormats, "pdftex","pdftex");
  set_value(OutputFormats, "dvips", "dvips");
  set_value(OutputFormats, "dvipsone", "dvipsone");
  set_value(OutputFormats, "acrobat", "acrobat");
  set_value(OutputFormats, "dviwindo", "dviwindo");
  set_value(OutputFormats, "dviview", "dviview");
  set_value(OutputFormats, "dvipdfm", "dvipdfm");
  set_value(OutputFormats, "dvipdfmx", "dvipdfmx");
  
  SetInterfaces( "nl", "dutch",        "dutch" );
  SetInterfaces( "en", "english",      "english" );
  SetInterfaces( "de", "german",       "german" );
  SetInterfaces( "cz", "czech",        "czech" );
  SetInterfaces( "uk", "brittish",     "english" );
  SetInterfaces( "it", "italian",      "italian" );
  SetInterfaces( "no", "norwegian",    "norwegian" );
  SetInterfaces( "ro", "romanian",     "romanian" );
  SetInterfaces( "xx", "experimental", "english" );
  
  int helpitems = 0;
  string *help = NULL;
  if (HelpAsked) {
    shorthelp = 0;    
    helpitems = keys (Help,&help);
    qsort(help,helpitems,sizeof(char *),&my_strcmp);
    if (ARGC == 1 && STREQ(ARGV[0],"all")) {
    } else if (ARGC == 1 && STREQ(ARGV[0],"short" )) {
      shorthelp--;
    } else if (ARGC == 1 && STREQ(ARGV[0],"help" )) {
      show_help_options();
      exit(0);
    } else if (ARGC) {
      string *h = ARGV;
      safe_free(help);
      help = malloc(sizeof(char *)*helpitems);
      string *testhelp;
      int testhelpk = keys (Help,&testhelp);
      helpitems = 0;
      int i,k;
      for (k=0;k<ARGC;k++) {
	char *a = safe_strdup(h[k]);
	if (strlen(a) && (a[0] == '*' || a[0] == '?')) 
	  my_concat(&a,".",a) ;
	if(do_get_value(Help,a)!=NULL) {
	  help[helpitems++] = a;
	} else { /* find any and all keys that contain a */
	  for (i=0;i<testhelpk;i++) {
	    if (strstr(testhelp[i],a) != NULL) {
	      help[helpitems++] = testhelp[i];
	    }
	  }
	}
      }
    } else {
      shorthelp++;
    }
  }

  if (get_item(ConTeXtInterfaces,ConTeXtInterface)) {
    ConTeXtInterface = hash_value(ConTeXtInterfaces,ConTeXtInterface);
  }
  if (strlen(SetFile)) { load_set_file( SetFile, Verbose ); }
  if    (HelpAsked)      { 
    show_help_info(helpitems,help);
  } else if (TypesetListing) {
    RunListing(ARGC,ARGV);
  } else if (TypesetFigures) {
    RunFigures(ARGC,ARGV);
  } else if (DoMPTeX) {
    RunMP(ARGC,ARGV);
  } else if (DoMPXTeX) {
    RunMPX( ARGV[0] );
  } else if (MakeFormats) {
    if (strlen(MpDoFormat)) {
      RunMpFormat(MpDoFormat) ;
    } else {
      RunFormats(ARGC,ARGV,fmtutil);
    }
  } else if (ARGC) {
    char *InputFile = safe_strdup(ARGV[0]);
    if (ARGC>1) {
      int i;
      for (i = 1; i < ARGC; i++) {
	my_concat3(&InputFile,InputFile," ",ARGV[i]);
      }
    }
    string *argv;
    int argc = CheckInputFiles(InputFile,&argv);
    if ((ARGC==1) && (argc==0)) {
      RunFiles(ARGC,ARGV);
    } else {
      RunFiles(argc,argv);
    }
  } else if (!HelpAsked ) {
    show_help_options();
  }
  TotalTime = time(NULL) - TotalTime;
  if (!HelpAsked) 
	fprintf(STDOUT, "\n        total run time : %d seconds\n" , (int)TotalTime);
  if (Problems) 
	return 1;
  return 0;
}
