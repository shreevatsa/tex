/* $Id: texutil.c,v 1.5 2004/03/17 13:57:30 taco Exp taco $
 *
 * This is a C port of Pragma's 'texutil.pl' perl script, v 8.2
 *
 * Not supported are:
 *  - Plugins            (the current texutil.pl assumes .pm files)
 *  - Remapped sort keys (disabled in context anyway)
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
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "perlemu.h"
#include "funclib.h"
#include "texutil.h"

#define UnknownOptions     TeXUtil->UnknownOptions_field
#define ProcessReferences  TeXUtil->ProcessReferences_field
#define ProcessIJ          TeXUtil->ProcessIJ_field
#define ProcessHigh        TeXUtil->ProcessHigh_field
#define ProcessQuotes      TeXUtil->ProcessQuotes_field
#define ProcessDocuments   TeXUtil->ProcessDocuments_field
#define ProcessSources     TeXUtil->ProcessSources_field
#define ProcessSetups      TeXUtil->ProcessSetups_field
#define ProcessTemplates   TeXUtil->ProcessTemplates_field
#define ProcessInfos       TeXUtil->ProcessInfos_field
#define ProcessFigures     TeXUtil->ProcessFigures_field
#define ProcessEpsPage     TeXUtil->ProcessEpsPage_field
#define ProcessEpsToPdf    TeXUtil->ProcessEpsToPdf_field
#define ProcessLogFile     TeXUtil->ProcessLogFile_field
#define ProcessBox         TeXUtil->ProcessBox_field
#define ProcessHBox        TeXUtil->ProcessHBox_field
#define ProcessVBox        TeXUtil->ProcessVBox_field
#define ProcessUnknown     TeXUtil->ProcessUnknown_field
#define PurgeFiles         TeXUtil->PurgeFiles_field
#define PurgeAllFiles      TeXUtil->PurgeAllFiles_field
#define AnalyzeFile        TeXUtil->AnalyzeFile_field
#define FilterPages        TeXUtil->FilterPages_field
#define SciteApi           TeXUtil->SciteApi_field
#define ProcessHelp        TeXUtil->ProcessHelp_field
#define ProcessSilent      TeXUtil->ProcessSilent_field
#define ProcessVerbose     TeXUtil->ProcessVerbose_field
#define TcXPath            TeXUtil->TcXPath_field
#define ProcessType        TeXUtil->ProcessType_field
#define ProcessOutputFile  TeXUtil->ProcessOutputFile_field
#define OutputFile         TeXUtil->OutputFile_field
#define ProcessCriterium   TeXUtil->ProcessCriterium_field
#define UserInterface      TeXUtil->UserInterface_field
#define InputFile          TeXUtil->InputFile_field
#define ProgramLog         TeXUtil->ProgramLog_field
#define STDOUT             TeXUtil->STDOUT_field
#define TUO                TeXUtil->TUO_field
#define SortN              TeXUtil->SortN_field
#define ExtraPrograms              TeXUtil->ExtraPrograms_field
#define ExtraPrograms_size	   TeXUtil->ExtraPrograms_size_field
#define ExtraPrograms_add	   TeXUtil->ExtraPrograms_add_field
#define NOfExtraPrograms	   TeXUtil->NOfExtraPrograms_field
#define RegisterEntry		   TeXUtil->RegisterEntry_field
#define RegisterEntry_size	   TeXUtil->RegisterEntry_size_field
#define RegisterEntry_add	   TeXUtil->RegisterEntry_add_field
#define SynonymEntry		   TeXUtil->SynonymEntry_field
#define SynonymEntry_size	   TeXUtil->SynonymEntry_size_field
#define SynonymEntry_add	   TeXUtil->SynonymEntry_add_field
#define Files			   TeXUtil->Files_field
#define File_opens		   TeXUtil->File_opens_field
#define Files_size		   TeXUtil->Files_size_field
#define Files_add		   TeXUtil->Files_add_field
#define NOfFigures		   TeXUtil->NOfFigures_field
#define Figures		       TeXUtil->Figures_field
#define Figures_size	       TeXUtil->Figures_size_field
#define Figures_add	       TeXUtil->Figures_add_field
#define NOfCommands	       TeXUtil->NOfCommands_field
#define NOfSynonyms	       TeXUtil->NOfSynonyms_field
#define NOfBadSynonyms         TeXUtil->NOfBadSynonyms_field
#define NOfEntries             TeXUtil->NOfEntries_field
#define NOfBadEntries          TeXUtil->NOfBadEntries_field
#define NOfFiles 	       TeXUtil->NOfFiles_field
#define NOfBadFiles            TeXUtil->NOfBadFiles_field
#define NOfPositionsFound      TeXUtil->NOfPositionsFound_field
#define TotalNOfPositions      TeXUtil->TotalNOfPositions_field
#define TotalNOfMPgraphics     TeXUtil->TotalNOfMPgraphics_field
#define MS                     TeXUtil->MS_field

#ifdef WIN32
static const char *gs = "gswin32c";
#else
static const char *gs = "gs";
#endif

#ifdef USE_KPSE
static const char *Program = "TeXUtilk 8.2 - ConTeXt / PRAGMA ADE 1992-2004/C port" ;
#else
static const char *Program = "TeXUtil 8.2 - ConTeXt / PRAGMA ADE 1992-2004/C port" ;
#endif

#define EXTEND_ARRAY(a,b)				     \
    if (a##_size < b) {                                      \
      a = safe_realloc(a,sizeof(char*)*(a##_size+a##_add));  \
      a##_size = a##_size+a##_add;                           \
    }

#define RegSep "::"

#define JOIN "__"
#define SPLIT "%%"

#define RegStat_f 1
#define RegStat_e 2
#define RegStat_t 3
#define RegStat_s 4
#define ASCII_hat   'a'
#define ASCII_quote 'b'
#define ASCII_back  'c'
#define ASCII_apos  'd'
#define ASCII_tilde 'e'
#define ASCII_comma 'f'
#define ishex(a) ((a>='0'&&a<=9) || (a>='a'&&a<='f'))
#define DPtoCM  (2.54/72.0) 
#define INtoCM  (2.54)


void RedirectTerminal (texutil TeXUtil) {
  safe_fopen(STDOUT,ProgramLog,"w");
}

void OpenTerminal (texutil TeXUtil) {
  safe_fclose(STDOUT);
  STDOUT = stdout;
}

void CloseTerminal (texutil TeXUtil) {
  fflush(STDOUT);
  safe_fopen(STDOUT,ProgramLog,"a");
}

void ShowBanner (texutil TeXUtil) {
  fprintf(STDOUT,"\n %s\n\n",Program);
}


void ShowHelpInfo (texutil TeXUtil) {
  fprintf(STDOUT,MS[ID_HelpInfo],ProgramLog);
  fprintf(STDOUT,"\n");
}

#define SPACES_TO_ZEROES(a)              \
  temp = a;                              \
  while(*temp) {                         \
    if (*temp == '\t' || *temp == ' ')   \
      *temp = '0';                       \
    temp++;                              \
  }

static int CompFileName (const void *a, const void *b) {
  char *fa;
  char *sa;
  char *fb;
  char *sb;
  char *temp;
  if (strstr((char *)a,".")) {
    sa = (char *)a;
    fa = next_word(&sa,".");
  } else {
    fa = (char *)a;
    sa = "";
  }
  if (strstr((char *)b,".")) {
    sb = (char *)b;
    fb = next_word(&sb,".");
  } else {
    fb = (char *)b;
    sb = "";
  }
  unsigned int ia, ib;
  if (strlen(sa) && strlen(sb) &&
      sscanf (sa,"%u",&ia) && sscanf (sb,"%u",&ib)) {
    sprintf ((char *)a,"%s.%10d",fa,ia);
    SPACES_TO_ZEROES((char *)a);
    sprintf ((char *)b,"%s.%10d",fb,ib);
    SPACES_TO_ZEROES((char *)b);
  }
  return strcmp(*((const char **)a), *((const char **)b));
}

static int CheckInputFiles (char *Input,string **iFiles) {
  int iNOfFiles = globfiles(Input,iFiles,GLOB_FILES);
  if (iNOfFiles>1)
    qsort(*iFiles,iNOfFiles,sizeof(char *),&CompFileName);
  return iNOfFiles;
}

char *SplitFileName (char **FullName) {
  char *Suffix;
  if ((strlen(*FullName)>1) && *FullName[0] == '.' && *FullName[1] == '/')
    *FullName+=2;
  Suffix = strrchr(*FullName,'.');
  if (Suffix != NULL) {
    *Suffix = 0;
    Suffix++;
  }
  return Suffix;
}

void RemoveContextFile (texutil TeXUtil,char *filename,int *removedfiles,int *persistentfiles,long int*reclaimedbytes) {
  long int filesize = file_size(filename);
  unlink (filename);
  if (file_exists(filename)) {
    ++*persistentfiles ;
    fprintf (STDOUT,"            persistent : %s\n", filename);
  } else{
    ++*removedfiles ;
    *reclaimedbytes += filesize ;
    fprintf (STDOUT,"               removed : %s\n", filename);
  }
}

#define KeepContextFile(a) { ++keptfiles ;                \
    fprintf (STDOUT,"                  kept : %s\n",a); }

void DoPurgeFiles (texutil TeXUtil,string *ARGV) {
  char *forsuresuffixes_a[] = { "tui", "tup", "ted", "tes", "top", "log", "tmp", "run", "bck",
				"rlg", "mpt", "mpx", "mpd", "mpo", NULL};
  char *forsuresuffixes_b[] = { "tui", "tup", "ted", "tes", "top", "log", "tmp", "run", "bck",
				"rlg", "mpt", "mpx", "mpd", "mpo", "tuo", "tub", "top", NULL};
  string *forsuresuffixes;

  char *texnonesuffixes_a[] = {"tuo","tub","top",NULL};
  char *texnonesuffixes_b[] = {NULL};
  string *texnonesuffixes;
  char *dontaskprefixes[] = { "tex-form.tex", "tex-edit.tex", "tex-temp.tex",
			      "texexec.tex", "texexec.tui",   "texexec.tuo",
			      "texexec.ps",  "texexec.pdf",   "texexec.dvi",
			      "cont-opt.tex", "cont-opt.bak", NULL };
  char *dontasksuffixes[] = { "mpgraph.mp", "mpgraph.mpd", "mpgraph.mpo",
			      "mpgraph.mpy","mprun.mp",  "mprun.mpd",
			      "mprun.mpo", "mprun.mpy", NULL };

  int removedfiles    = 0 ;
  int persistentfiles = 0 ;
  long int reclaimedbytes  = 0 ;
  char *pattern =NULL;
  char *strippedname ;
  int keptfiles=0;
  string *files;
  string *dontaskmpxprefixes;
  int k;
  int r;
  if (ARGV[0] == NULL || (!strlen(ARGV[0]))) {
    pattern = safe_strdup("*.*") ;
    CheckInputFiles(pattern, &files);
  } else {
    my_concat(&pattern, ARGV[0], "*.* " );
    my_concat3(&pattern, pattern, ARGV[0], "-*.* " );
    CheckInputFiles(pattern, &files);
  }
  fprintf (STDOUT,"         purging files : %s\n\n", pattern);
  safe_free(pattern);
  CheckInputFiles("mpx-*",&dontaskmpxprefixes);
  if (PurgeAllFiles) {
    forsuresuffixes = forsuresuffixes_b;
    texnonesuffixes = texnonesuffixes_b;
  } else {
    forsuresuffixes = forsuresuffixes_a;
    texnonesuffixes = texnonesuffixes_a;
  }
  k = 0;
  while(dontaskmpxprefixes[k] != NULL) {
    if (file_exists(dontaskmpxprefixes[k]))
      RemoveContextFile(TeXUtil,dontaskmpxprefixes[k],&removedfiles,&persistentfiles,&reclaimedbytes);
    safe_free(dontaskmpxprefixes[k]);
    k++;
  }
  safe_free(dontaskmpxprefixes);
  k = 0;
  while(dontaskprefixes[k] != NULL) {
    if (file_exists(dontaskprefixes[k]))
      RemoveContextFile(TeXUtil,dontaskprefixes[k],&removedfiles,&persistentfiles,&reclaimedbytes);
    k++;
  }
  k = 0;
  while(dontasksuffixes[k] != NULL) {
    if (file_exists(dontasksuffixes[k]))
      RemoveContextFile(TeXUtil,dontasksuffixes[k],&removedfiles,&persistentfiles,&reclaimedbytes);
    k++;
  }
  k = 0;
  while(dontasksuffixes[k] != NULL) {
    r = 0;
    while(files[r] != NULL) {
      char *tester = safe_strdup(files[r]);
      tester = lc (tester);
      char *temp = strstr(tester,dontasksuffixes[k]);
      if (temp != NULL && *(temp+strlen(temp)) == 0) {
		RemoveContextFile(TeXUtil,files[r],&removedfiles,&persistentfiles,&reclaimedbytes);
      }
      safe_free(tester);
      r++;
    }
    k++;
  }
  k = 0;
  while(forsuresuffixes[k] != NULL) {
    r = 0;
    while(files[r] != NULL) {
      char *tester = safe_strdup(files[r]);
      tester = lc(tester);
      char *tester2 = strrchr(tester,'.');
      if (tester2 != NULL && strstr(tester2,forsuresuffixes[k]) != NULL) {
	RemoveContextFile(TeXUtil,files[r],&removedfiles,&persistentfiles,&reclaimedbytes);
      }
      safe_free(tester);
      r++;
    }
    k++;
  }
  r = 0;
  while(files[r] != NULL) {
    char *tester = safe_strdup(files[r]);
    char *testerpointer = tester;
    if (strstr(tester,".")) {
      tester = strrchr(tester,'.');
      tester++;
      while (*tester && isdigit(*tester))
	tester++;
      if (!*tester)
	RemoveContextFile(TeXUtil,files[r],&removedfiles,&persistentfiles,&reclaimedbytes);
    }
    safe_free(testerpointer);
    r++;
  }
  k = 0;
  while(texnonesuffixes[k]  != NULL) {
    r = 0;
    while((files[r] != NULL) && strlen(files[r])) {
      char *tester = safe_strdup(files[r]);
      tester = lc(tester);
      char *testerpointer = tester;
      char *temp = strrchr(tester,'.');
      if (temp == tester)
		temp = NULL;
      if (temp != NULL) {
		char *temp2 = NULL;
		my_concat(&temp2,".",texnonesuffixes[k]);
		temp = strstr(temp,temp2);
		safe_free(temp2);
      }
      if (temp != NULL) {
		*temp = 0;
		char *tempname = safe_strdup(files[r]);
		*(tempname + strlen(tester)) = 0;
		char *testa=NULL;
		my_concat(&testa,tempname,".tex");
		char *testb=NULL;
		my_concat(&testb,tempname,".xml");
		if (file_exists(testa)||file_exists(testb)) {
		  KeepContextFile(files[r]);
		} else {
		  strippedname = tempname ;
		  int kk = strlen(strippedname);
		  while(kk>0 && isalpha(strippedname[kk])) {
			kk--;
		  }
		  if (strippedname[kk]== '-')
			strippedname[kk] = 0;
		  my_concat(&testa,strippedname,".tex");
		  my_concat(&testb,strippedname,".xml");
		  if (file_exists(testa)||file_exists(testb)) {
			char *tempfile = NULL;
			my_concat(&tempfile,files[r]," (potential result file)");
			KeepContextFile(tempfile);
			safe_free(tempfile);
		  } else {
			RemoveContextFile(TeXUtil,files[r],&removedfiles,&persistentfiles,&reclaimedbytes);
		  }
		}
		safe_free(tempname);
		safe_free(testa);
		safe_free(testb);
      }
	  safe_free(testerpointer);
	  safe_free(files[r]);
      r++;
    }
    safe_free(files);
    k++;
  }
  if (removedfiles||keptfiles||persistentfiles)
    fprintf(STDOUT,"\n" );
  fprintf(STDOUT, "         removed files : %d\n" , removedfiles);
  fprintf(STDOUT, "            kept files : %d\n" , keptfiles);
  fprintf(STDOUT, "      persistent files : %d\n" , persistentfiles);
  fprintf(STDOUT, "       reclaimed bytes : %ld\n" , reclaimedbytes);
}


char *SetOutputFile (texutil TeXUtil,char *proposeda) {
  char *OutFilNam;
  char *OutFilSuf="";
  char *FilSuf = NULL;
  char *proposed = safe_strdup(proposeda);
  char *proposedpointer = proposed;
  if (strchr(proposed,'.') != NULL) {
    OutFilNam = next_word(&proposed,".");
    OutFilSuf = proposed;
  } else {
    OutFilNam = proposed;
  }
  if (strlen(ProcessOutputFile)) {
    my_concat(&FilSuf,".",OutFilSuf);
    my_concat(&ProcessOutputFile,ProcessOutputFile,FilSuf);
    char *Rubish = ProcessOutputFile;
    OutFilNam = next_word(&Rubish,".");
    if (strchr(Rubish,'.') != NULL) {
      OutFilSuf = next_word(&Rubish,".");
    } else {
      OutFilSuf = Rubish;
    }
  }
  my_concat(&FilSuf,".",OutFilSuf);
  my_concat(&OutputFile,OutFilNam,FilSuf);
  safe_free(proposedpointer);
  return OutputFile;
}

void HandleEditorCues (texutil TeXUtil) {
  char *FileName;
  char *FileSuffix;
  int k;
  char *inputline=NULL;
  char *SomeLine;
  char *Tag;
  char *RestOfLine;
  int NOfTemplates ;
  int NOfInfos ;
  int NOfSuppliedFiles;
  string *UserSuppliedFiles;
  FILE *TEX;
  FILE *TUD;
  if (ProcessTemplates)
    fprintf(STDOUT,"%s %s\n", MS[ID_Action], MS[ID_CopyingTemplates]) ;
  if (ProcessInfos)
    fprintf(STDOUT,"%s %s\n", MS[ID_Action], MS[ID_CopyingInformation]) ;
  if (!strlen(InputFile)) {
    fprintf(STDOUT,"%s %s\n", MS[ID_Error], MS[ID_NoInputFile]) ;
  } else {
    NOfSuppliedFiles = CheckInputFiles (InputFile,&UserSuppliedFiles) ;
    if (NOfSuppliedFiles)
      inputline = safe_malloc(BUFSIZE);
    for (k=0; k<NOfSuppliedFiles; k++) {
      FileName = UserSuppliedFiles[k];
      FileSuffix = SplitFileName (&FileName) ;
      if (FileSuffix == NULL)
	FileSuffix ="tex";
      char *testfile = NULL;
      my_concat3(&testfile,FileName,".",FileSuffix);
      if (!(file_exists(testfile)))
	continue;
      safe_fopen (TEX,testfile,"r");
      safe_free(testfile);
      if (!TEX) {
	fprintf(STDOUT,"%s %s %s.%s\n", MS[ID_Error], MS[ID_EmptyInputFile],FileName,FileSuffix) ;
      } else {
	fprintf(STDOUT,"%s %s.%s\n", MS[ID_InputFile],FileName,FileSuffix) ;
	fprintf(STDOUT,"%s %s.tud\n", MS[ID_OutputFile],FileName) ;
	char *tudfile = NULL;
	my_concat(&tudfile,FileName,".tud");
	safe_fopen (TUD,tudfile,"w") ;
	if (!TUD)
	  continue;
	NOfTemplates = 0 ;
	NOfInfos = 0 ;
	while (readline(TEX,&inputline)) {
	  SomeLine = chomp(inputline);
	  RestOfLine = strchr(SomeLine,' ');
	  if (RestOfLine != NULL) {
	    Tag = SomeLine;
	    *RestOfLine = 0;
	    RestOfLine++;
	  } else {
	    Tag = SomeLine;
	    RestOfLine = "";
	  }
	  if ((STREQ(Tag,"%T")) && ProcessTemplates) {
	    ++NOfTemplates ;
	    while (STREQ(Tag,"%T")){
	      fprintf (TUD , "%s\n", SomeLine) ;
	      readline(TEX,&inputline);
	      SomeLine = chomp (inputline);
	      RestOfLine = strchr(SomeLine,' ');
	      if (RestOfLine != NULL) {
		Tag = SomeLine;
		*RestOfLine = 0;
		RestOfLine++;
	      } else {
		Tag = SomeLine;
		RestOfLine = "";
	      }
	    }
	    fprintf (TUD , "\n") ;
	  } else if ((STREQ(Tag,"%I")) && ProcessInfos) {
	    ++NOfInfos ;
	    while ((STREQ(Tag,"%I")) || (STREQ(Tag,"%P"))) {
	      fprintf (TUD , "%s\n", SomeLine) ;
	      readline(TEX,&inputline);
	      SomeLine = chomp (inputline);
	      RestOfLine = strchr(SomeLine,' ');
	      if (RestOfLine != NULL) {
		Tag = SomeLine;
		*RestOfLine = 0;
		RestOfLine++;
	      } else {
		Tag = SomeLine;
		RestOfLine = "";
	      }
	    }
	    fprintf (TUD , "\n") ;
	  }
	}
	safe_fclose (TUD) ;
	if (!(NOfTemplates||NOfInfos))
	  unlink (tudfile);
	if (ProcessTemplates)
	  fprintf(STDOUT,"%s %d\n",MS[ID_NOfTemplates], NOfTemplates);
	if (ProcessInfos)
	  fprintf(STDOUT,"%s %d\n",MS[ID_NOfInfos], NOfInfos);
	safe_free(tudfile);
      }
      safe_fclose(TEX);
      safe_free(UserSuppliedFiles[k]);
    }
    safe_free(UserSuppliedFiles);
    safe_free(inputline);
  }
}


void HandleSetups (texutil TeXUtil) {
  char *FileName;
  char *FileSuffix;
  int k;
  char *inputline=NULL;
  char *SomeLine;
  char *Tag;
  char *RestOfLine;
  int NOfSetups;
  FILE *TEX;
  FILE *TUS;
  OutputFile=NULL;
  int NOfSuppliedFiles;
  string *UserSuppliedFiles;
  fprintf(STDOUT,"%s %s\n", MS[ID_Action], MS[ID_FilteringDefinitions]) ;
  if (!strlen(InputFile)) {
    fprintf(STDOUT,"%s %s\n", MS[ID_Error], MS[ID_NoInputFile]) ;
  } else {
    OutputFile = SetOutputFile (TeXUtil,"texutil.tus") ;
    safe_fopen (TUS,OutputFile,"w") ;
    if (!TUS)
      return;
    fprintf(STDOUT,"%s %s\n", MS[ID_OutputFile], OutputFile) ;
    NOfSetups = 0 ;
    NOfSuppliedFiles = CheckInputFiles (InputFile,&UserSuppliedFiles) ;
    if (NOfSuppliedFiles)
      inputline = safe_malloc(BUFSIZE);
    for (k=0; k<NOfSuppliedFiles; k++) {
      FileName = UserSuppliedFiles[k];
      FileSuffix = SplitFileName (&FileName) ;
      if (FileSuffix == NULL)
	FileSuffix ="tex";
      char *texfile = NULL;
      my_concat3(&texfile,FileName,".",FileSuffix);
      if (!(file_exists(texfile)))
	continue;
      safe_fopen (TEX,texfile,"r");
      safe_free(texfile);
      if (!TEX) {
	fprintf(STDOUT,"%s %s %s.%s\n", MS[ID_Error], MS[ID_EmptyInputFile],FileName,FileSuffix) ;
      } else {
	fprintf(STDOUT,"%s %s.%s\n", MS[ID_InputFile],FileName,FileSuffix) ;
	fprintf(TUS, "%%\n%% File : %s.%s\n%%\n",FileName,FileSuffix );
	while (readline(TEX,&inputline)) {
	  SomeLine = chomp(inputline) ;
	  RestOfLine = strchr(SomeLine,' ');
	  if (RestOfLine != NULL) {
	    Tag = SomeLine;
	    *RestOfLine = 0;
	    RestOfLine++;
	    if (STREQ(Tag,"%S")) {
	      ++NOfSetups ;
	      while (STREQ(Tag,"%S")){
		fprintf (TUS, "%s\n",RestOfLine) ;
		readline(TEX,&inputline) ;
		SomeLine =chomp(inputline);
		Tag = SomeLine;
		RestOfLine = Tag+3;
		if (Tag[2] == ' ' )
		  Tag[2] = 0;
	      }
	      fprintf (TUS,"\n");
	    }
	  }
	}
	safe_fclose(TEX);
      }
      safe_free(UserSuppliedFiles[k]);
    }
    safe_free(UserSuppliedFiles);
    safe_free(inputline);
    safe_fclose (TUS) ;
    if (!NOfSetups)
      unlink (OutputFile);
    safe_free(OutputFile);
    fprintf(STDOUT,"%s %d\n",MS[ID_NOfSetups], NOfSetups);
  }
}



void HandleSources (texutil TeXUtil)   {
  int k;
  char *FileSuffix="";
  char *FileName;
  FILE *TEX;
  FILE *TES;
  char *inputline=NULL;
  char *SomeLine;
  int EmptyLineDone = 0;
  int FirstCommentDone = 0;
  int NOfSuppliedFiles;
  string *UserSuppliedFiles;
  fprintf(STDOUT,"%s %s\n", MS[ID_Action], MS[ID_GeneratingSources]) ;
  if (!strlen(InputFile)) {
    fprintf(STDOUT,"%s %s\n", MS[ID_Error], MS[ID_NoInputFile]) ;
  } else {
    NOfSuppliedFiles = CheckInputFiles (InputFile,&UserSuppliedFiles) ;
    if (NOfSuppliedFiles)
      inputline = safe_malloc(BUFSIZE);
    for (k=0; k<NOfSuppliedFiles; k++) {
      FileName = UserSuppliedFiles[k];
      FileSuffix = SplitFileName (&FileName) ;
      if (FileSuffix == NULL)
	FileSuffix ="tex";
      char *texfile = NULL;
      my_concat3(&texfile,FileName,".",FileSuffix);
      if (!(file_exists(texfile)))
	continue;
      safe_fopen (TEX,texfile,"r");
      safe_free(texfile);
      if (!TEX) {
	fprintf(STDOUT,"%s %s %s.%s\n", MS[ID_Error], MS[ID_EmptyInputFile],FileName,FileSuffix) ;
      } else {
	fprintf(STDOUT,"%s %s.%s\n", MS[ID_InputFile],FileName,FileSuffix) ;
	char *tesfile = NULL;
	my_concat(&tesfile,FileName,".tes");
	safe_fopen (TES,tesfile,"w") ;
	if (!TES) {
	  safe_free(tesfile);
	  continue;
	}
	fprintf(STDOUT,"%s %s.tes\n", MS[ID_OutputFile],FileName) ;
	EmptyLineDone = 1 ;
	FirstCommentDone = 0 ;
	while (readline(TEX,&inputline)) {
	  SomeLine = chomp(inputline);
	  if (!strlen(SomeLine )) {
	    if (!FirstCommentDone) {
	      FirstCommentDone = 1 ;
	      fprintf (TES,"\n%% further documentation is removed\n\n" );
	      EmptyLineDone = 1;
	    }
	    if (!EmptyLineDone) {
	      fprintf (TES, "\n" );
	      EmptyLineDone = 1;
	    }
	  } else if (SomeLine[0] == '%') {
	    if (!FirstCommentDone) {
	      fprintf (TES, "%s\n", SomeLine) ;
	      EmptyLineDone = 0;
	    }
	  } else {
	    fprintf (TES, "%s\n", SomeLine) ;
	    EmptyLineDone = 0 ;
	  }
	}
	safe_fclose (TES);
	safe_fclose(TEX);
        if (!FirstCommentDone)
	  unlink (tesfile);
	safe_free(tesfile);
      }
      safe_free(UserSuppliedFiles[k]);
    }
    safe_free(inputline);
    safe_free(UserSuppliedFiles);
  }
}


void HandleDocuments (texutil TeXUtil) {
  int k;
  char *FileSuffix="";
  char *FileName;
  FILE *TEX;
  FILE *TED;
  char *FileType;
  char *inputline=NULL;
  char *SomeLine;
  int NOfDocuments   = 0 ;
  int NOfDefinitions = 0 ;
  int NOfSkips       = 0 ;
  int SkipLevel      = 0 ;
  int InDocument     = 0 ;
  int InLocalDocument = 0 ;
  int InDefinition   = 0 ;
  int skippingbang = 0;
  string *UserSuppliedFiles;
  int NOfSuppliedFiles = 0;
  fprintf(STDOUT,"%s %s\n", MS[ID_Action], MS[ID_GeneratingDocumentation]) ;
  if (strlen(ProcessType)) {
    fprintf(STDOUT,"%s %s %s\n", MS[ID_Option], MS[ID_ForcingFileType], ProcessType) ;
  }
  if (!strlen(InputFile)) {
    fprintf(STDOUT,"%s %s\n", MS[ID_Error], MS[ID_NoInputFile]) ;
  } else {
    NOfSuppliedFiles = CheckInputFiles (InputFile,&UserSuppliedFiles) ;
    if (NOfSuppliedFiles)
      inputline = safe_malloc(BUFSIZE);
    for (k=0; k<NOfSuppliedFiles; k++) {
      FileName = UserSuppliedFiles[k];
      FileSuffix = SplitFileName(&FileName);
      if (FileSuffix == NULL)
	FileSuffix ="tex";
      char *texfile = NULL;
      my_concat3(&texfile,FileName,".",FileSuffix);
      if (!(file_exists(texfile))) {
	safe_free(texfile);
	continue;
      }
      safe_fopen (TEX,texfile,"r");
      safe_free(texfile);
      if (!TEX) {
	fprintf(STDOUT,"%s %s %s.%s\n", MS[ID_Error], MS[ID_EmptyInputFile],FileName,FileSuffix) ;
      } else {
	fprintf(STDOUT,"%s %s.%s\n", MS[ID_InputFile],FileName,FileSuffix) ;
	fprintf(STDOUT,"%s %s.ted\n", MS[ID_OutputFile],FileName) ;
	char *tedfile = NULL;
	my_concat(&tedfile,FileName,".ted");
	safe_fopen (TED,tedfile,"w") ;
	if (!TED) {
	  safe_free(tedfile);
	  continue;
	}
	NOfDocuments   = 0 ;
	NOfDefinitions = 0 ;
	NOfSkips       = 0 ;
	SkipLevel      = 0 ;
	InDocument     = 0 ;
	InDefinition   = 0 ;
	if (!strlen(ProcessType)) {
	  FileType=safe_strdup(FileSuffix);
	} else {
	  FileType=safe_strdup(ProcessType);
	}
	FileType = lc(FileType);
	fprintf(STDOUT,"%s %s\n", MS[ID_FileType], FileType) ;
	/* we need to signal to texexec what interfaec to use*/
	readline(TEX,&inputline);
	if (inputline[0] == '%' && (strstr(inputline,"interface=") != NULL)) {
	  fprintf (TED, "%s",inputline );
	} else {
	  fseek (TEX, 0, 0 );
	}
	/* # so far*/
	fprintf (TED,"\\startmodule[type=%s]\n",FileType) ;
	safe_free(FileType);
	while (readline(TEX,&inputline)) {
	  inputline = chomp(inputline) ;
	  inputline = trim(inputline);
	  if (skippingbang) {
	    skippingbang = 0 ;
	  } else if (strlen(inputline)>1 && inputline[1] == 'D'  &&
		     (inputline[0] == '%' || inputline[0] == '#')) {
	    if (SkipLevel == 0) {
	      if (strlen (inputline) < 3) {
		SomeLine = "" ;
	      } else { /*               # HH: added after that*/
		SomeLine = inputline+3 ;
	      };
	      if (InDocument) {
		fprintf (TED, "%s\n", SomeLine) ;
	      } else {
		if (InDefinition) {
		  fprintf (TED,"\\stopdefinition\n") ;
		  InDefinition = 0;
		}
	        if (!InDocument) {
		  fprintf (TED, "\n\\startdocumentation\n" );
		}
		fprintf (TED,"%s\n", SomeLine) ;
		InDocument = 1 ;
		++NOfDocuments;
	      }
	    }
	  } else if (strlen(inputline)>1 && inputline[1] == 'M'
		     &&
		     (inputline[0] == '%' || inputline[0] == '#')) {
	    if (SkipLevel == 0) {
	      SomeLine = inputline+3 ;
	      fprintf (TED,"%s\n", SomeLine) ;
	    }
	  } else if (strlen(inputline)>3 && (strncmp(inputline,"%S B",4)==0)) {
	    ++SkipLevel;
	    ++NOfSkips;
	  } else if (strlen(inputline)>3 && (strncmp(inputline,"%S E",4)==0)) {
	    --SkipLevel;
          } else if (strlen(inputline)>0 && (inputline[0] == '%' || inputline[0] == '#')) {
	    ;
          } else if (strlen(inputline)>strlen("eval '(exit $?0)' && eval 'exec perl") &&
		     strstr(inputline,"eval '(exit $?0)' && eval 'exec perl") == inputline) {
            skippingbang = 1 ;
          } else if (SkipLevel == 0) {
	    InLocalDocument = InDocument ;
            SomeLine = inputline ;
	    if (InDocument) {
	      fprintf (TED,"\\stopdocumentation\n" );
	      InDocument = 0;
	    }
	    if ((!strlen(SomeLine)) && InDefinition) {
	      fprintf (TED,"\\stopdefinition\n" );
	      InDefinition = 0;
	    } else {
	      if (InDefinition) {
		fprintf (TED, "%s\n",SomeLine );
	      } else if (strlen(SomeLine)) {
		fprintf (TED,"\n\\startdefinition\n") ;
		InDefinition = 1 ;
		if (!InLocalDocument) {
		  ++NOfDefinitions;
		}
		fprintf (TED, "%s\n", SomeLine);
	      }
	    }
	  }
	}
	if (InDocument)
	  fprintf (TED, "\\stopdocumentation\n" );
	if (InDefinition)
	  fprintf (TED, "\\stopdefinition\n" );
	fprintf (TED,"\\stopmodule\n" );
	safe_fclose (TED) ;
	safe_fclose(TEX);
	if (!(NOfDocuments||NOfDefinitions))
	  unlink (tedfile);
	safe_free(tedfile);
	fprintf(STDOUT,"%s %d\n", MS[ID_NOfDocuments], NOfDocuments) ;
	fprintf(STDOUT,"%s %d\n", MS[ID_NOfDefinitions], NOfDefinitions) ;
	fprintf(STDOUT,"%s %d\n", MS[ID_NOfSkips], NOfSkips);
      }
      safe_free(UserSuppliedFiles[k]);
    }
    safe_free(inputline);
    safe_free(UserSuppliedFiles);
  }
}

/* main goal of the InitializeKeys() routine is to discover any tcx files,
 * and if yes, set our Filter array according to it's contents.
 *
 * @paths only existed to discover kpsewhich which
 * in turn would find the tcx filename itself.
 *
 */

void
InitializeKeys (texutil TeXUtil,string *ARGV) {
  char *filename = NULL;
  int i;
  FILE *TEX;
  int Filter[256];
  my_concat (&filename,ARGV[0],".tex");
  safe_fopen (TEX,filename,"r");
  safe_free(filename);
  if (!TEX)
    return;
  for (i=0;i<=255;i++) {
    Filter[i] = i ;
  }
  char *line = safe_malloc(BUFSIZE);
  char *linepointer = line;
  readline(TEX,&line);
  line = chomp(line);
  char *Filter_file =NULL;
  if (line && *line == '%') {
    if((line=strstr(line,"translation"))) {
      if((line=strstr(line,"="))) {
	char *Translation = safe_strdup(++line);
	char *rover = Translation;
	while ((*rover >= 'a' && *rover <= 'z') ||
	       (*rover >= 'A' && *rover <= 'Z') ||
	       (*rover >= '0' && *rover <= '9') ||
	       (*rover == ':') || (*rover == '/') || (*rover == '-')) {
	  rover++;
	}
	*rover = 0;
	if (strlen(TcXPath)) {
	  TcXPath = checked_path(TcXPath) ;
	  my_concat3(&Filter_file,TcXPath,Translation,".tcx");
	} else {
	  my_concat(&Translation,Translation,".tcx");
	  Filter_file = discover_file(Translation,"web2c files");
	}
	FILE *ASC;
	safe_fopen(ASC,Filter_file,"r");
	if (ASC) {
	  fprintf(STDOUT,"%s %s\n",MS[ID_LoadedFilter],Translation);
	  char *tcxline = safe_malloc(BUFSIZE);
	  int a,b;
	  while (readline(ASC,&tcxline)) {
	    if(sscanf(tcxline,"%d %d",&a,&b)==2)
	      Filter[b] = a;
	  }
	  safe_fclose (ASC);
	  safe_free (tcxline);
	} else if (strlen(TcXPath)) {
	  fprintf(STDOUT,"%s %s\n",MS[ID_WrongFilterPath], TcXPath);
	}
	safe_free(Filter_file);
      }
    }
  }
  safe_free(linepointer);
  safe_fclose (TEX) ;
}

#define TRUNC_TO_NEXT()    *kick = 0;  \
   RestOfLine = kick+1;                \
   while (isspace(*RestOfLine))        \
     RestOfLine++;                     \
   kick = strchr(RestOfLine,'}')


void HandleKey (texutil TeXUtil,char *RestOfLine) {
   ++SortN ;
   char *STR[256];
   char *CHR[256];
   char *MAP[256];
   //   char *ALF[256];
   char *kick;
   RestOfLine = strchr(RestOfLine,'{')+1;
   if (RestOfLine == NULL)
     return;
   kick = strrchr(RestOfLine,'}');
   if (kick == NULL)
     return;
   *kick = 0;
   char *lan;
   char *enc;
   char *str;
   char *chr;
   char *map;
   char *alf;
   kick = strchr(RestOfLine,'}');
   lan = RestOfLine;
   TRUNC_TO_NEXT();
   enc = RestOfLine+1;
   TRUNC_TO_NEXT();
   str = RestOfLine+1;
   TRUNC_TO_NEXT();
   chr = RestOfLine+1;
   TRUNC_TO_NEXT();
   map = RestOfLine+1;
   *kick = 0;
   alf = kick+1;
   while (isspace(*RestOfLine))
     RestOfLine++;
   STR[SortN] = str ;
   CHR[SortN] = chr ;
   MAP[SortN] = map ;
}

void FlushKeys (texutil TeXUtil) {
  fprintf(STDOUT,"%s %d\n",MS[ID_RemappedKeys],SortN);
}



void HandleCommand (texutil TeXUtil, char *RestOfLine)  {
  ++NOfCommands ;
  char *Item;
  while (RestOfLine[0] == '\\') RestOfLine++;
  if (strstr(RestOfLine,"pospxy") == RestOfLine) {
    ++NOfPositionsFound ;
  } else if (RestOfLine[0] == 'i'){
    if (strstr(RestOfLine,"initializevariable\\totalnofpositions{") == RestOfLine) {
      Item =RestOfLine + strlen("initializevariable\\totalnofpositions{");
      while (*Item == ' ')
	Item++;
      TotalNOfPositions = atoi(Item);
    } else {
      if (strstr(RestOfLine,"initializevariable\\totalnofMPgraphics{") == RestOfLine)  {
	Item = RestOfLine+strlen("initializevariable\\totalnofMPgraphics{");
	while (*Item == ' ')
	  Item++;
	TotalNOfMPgraphics = atoi(Item);
      }
    }
  }
  fprintf (TUO,"\\%s\n",RestOfLine);
}


void FlushCommands (texutil TeXUtil)  {
  fprintf(STDOUT,"%s %d\n",MS[ID_PassedCommands],NOfCommands);
}


void HandleExtra (texutil TeXUtil,char *RestOfLine) {
  char *kick;
  if (strlen(RestOfLine)<2)
    return;
  if(RestOfLine[0] == 'p' && (RestOfLine[1] == ' ' || RestOfLine[1] == '\t')) {
    RestOfLine+=2;
    while(*RestOfLine == ' ' || *RestOfLine == '\t')
      RestOfLine++;
    RestOfLine = strchr(RestOfLine,'{')+1;
    if (RestOfLine == NULL)
      return;
    kick = strrchr(RestOfLine,'}');
    if (kick == NULL)
      return;
    *kick = 0;
    EXTEND_ARRAY(ExtraPrograms,NOfExtraPrograms);
    ExtraPrograms[NOfExtraPrograms] = safe_strdup(RestOfLine);
    NOfExtraPrograms++;
  }
}

void FlushExtra (texutil TeXUtil) {
  int i;
  fprintf (TUO, "%%\n%% %s / System\n%%\n" , Program);
  for (i=0;i<NOfExtraPrograms;i++) {
    fprintf (TUO, "%% extra program : %s\n",ExtraPrograms[i]);
  }
}

void RunExtraPrograms (texutil TeXUtil) {
  int i;
  for (i=0;i<NOfExtraPrograms;i++) {
    fprintf(STDOUT,"%s %s\n",MS[ID_ExtraProgram],ExtraPrograms[i]) ;
    system(ExtraPrograms[i]);
    safe_free(ExtraPrograms[i]);
    /* Do this? Perl version doesnt
       if (WIFSIGNALED(ret) &&
       (WTERMSIG(ret) == SIGINT || WTERMSIG(ret) == SIGQUIT))
       break;
    */
  }
  safe_free(ExtraPrograms);
}

/* char *SectionSeparator = ":" ; */


#define ESCAPED_CHAR(a,b)                 \
  NewKey = safe_malloc(strlen(a)*2);      \
  apointer = a;                           \
  rover= 0;                               \
  while (*a) {                            \
    if (*a == '\\' && *(a+1) == b) {	  \
      NewKey[rover++] = '\\';		  \
      a++;                                \
      NewKey[rover++] = *a;		  \
    } else if (*a == b) {                 \
      NewKey[rover++] = '%';		  \
      NewKey[rover++] = '%';		  \
    } else {                              \
      NewKey[rover++] = *a;               \
    }                                     \
    a++;                                  \
  }                                       \
  NewKey[rover] = 0;                      \
  safe_free(apointer);		          \
  a = NewKey


#define ESCAPED_IJ(a)                     \
  NewKey = safe_malloc(strlen(a)*2);      \
  apointer = a;                           \
  rover= 0;                               \
  while (*a) {                            \
    if (*a == 'i' && *(a+1) == 'j') {     \
          NewKey[rover++] = 'y';          \
          a++;                            \
	  NewKey[rover++] = 'y';          \
    } else {                              \
          NewKey[rover++] = *a;           \
    }                                     \
    a++;                                  \
  }                                       \
  NewKey[rover] = 0;                      \
  safe_free(apointer);                    \
  a = NewKey


#define STRIP_PRE_SPACE_AND_BRACES(Key)    \
  if ((strchr(Key,'{') != NULL)             \
      &&                                   \
      (strchr(Key,'}') != NULL)             \
      &&                                   \
      (strchr(Key,'{') < strchr(Key,'}'))) { \
    while (isspace(*Key))                  \
      Key++;                               \
    if (*Key == '{') {                     \
      Key++;                               \
    }                                      \
    char *End = strrchr(Key,'}');           \
    *End = 0;                              \
    End++;                                 \
    my_concat(&Key,Key,End);		   \
  }

/*    inputstring =~ s/\\([\^\"\`\'\~\,])/$1/g ;*/
#define DROP_ACCENT_BACKSLASHES(a)             \
  NewKey = safe_malloc(strlen(a)*2);           \
  apointer = a;                                \
  rover= 0;                                    \
  PrevChar=0;                                  \
  while (*a) {                                 \
    if (*a == '^' || *a == '"' ||*a == '`' ||  \
        *a == '\'' || *a == '~' ||*a == ',') { \
      if (PrevChar != '\\') {                  \
	 NewKey[rover++] = PrevChar;           \
      }                                        \
    } else {                                   \
      NewKey[rover++] = *a;                    \
    }                                          \
    PrevChar = *a;                             \
    a++;                                       \
  }                                            \
  if (PrevChar != '\\') {		       \
    NewKey[rover++] = PrevChar;		       \
  }					       \
  NewKey[rover] = 0;                           \
  safe_free(apointer);                         \
  a = NewKey

/*  inputstring =~ s/\\-|\|\|/\-/gio ; */
#define MAKE_DASHES(a)                         \
  NewKey = safe_malloc(strlen(a)+1);           \
  apointer = a;                                \
  rover= 0;                                    \
  while (*a) {                                 \
    if (*a == '\\' && *(a+1)=='-' ) {          \
      a++;                                     \
      NewKey[rover++] = *a;                    \
    } else if (*a == '|' && *(a+1)=='|' ){     \
      a++;                                     \
      NewKey[rover++] = '-';                   \
    } else {                                   \
      NewKey[rover++] = *a;                    \
    }                                          \
    a++;                                       \
  }                                            \
  NewKey[rover] = 0;                           \
  safe_free(apointer);                         \
  a = NewKey



/*    copied =~ s/([\^\"\`\'\~\,])([a-zA-Z])/$ASCII{$1}/gi ;*/

#define DROP_ACCENTED_CHARS(a)                 \
  NewKey = safe_malloc(strlen(a)+1);           \
  apointer = a;                                \
  rover= 0;                                    \
  while (*a) {                                 \
    if (*a == '^' && isalpha(a[1])) {	       \
      NewKey[rover++] = ASCII_hat;  a++;       \
    } else if (*a == '"' && isalpha(a[1])) {   \
      NewKey[rover++] = ASCII_quote;  a++;     \
    } else if (*a == '`' && isalpha(a[1])) {   \
      NewKey[rover++] = ASCII_back;  a++;      \
    } else if (*a == '\'' && isalpha(a[1])) {  \
      NewKey[rover++] = ASCII_apos;   a++;     \
    } else if (*a == '~' && isalpha(a[1])) {   \
      NewKey[rover++] = ASCII_tilde;  a++;     \
    } else if (*a == ',' && isalpha(a[1])) {   \
      NewKey[rover++] = ASCII_comma;  a++;     \
    } else {				       \
      NewKey[rover++] = *a;                    \
    }                                          \
    a++;                                       \
  }                                            \
  NewKey[rover] = 0;                           \
  safe_free(apointer);                         \
  a = NewKey


/*    inputstring =~ s/([\^\"\`\'\~\,])([a-zA-Z])/$2/gio ;*/
#define DROP_ACCENTS(a)                        \
  NewKey = safe_malloc(strlen(a)+1);           \
  apointer = a;                                \
  rover= 0;                                    \
  while (*a) {                                 \
    if (*a == '^' && isalpha(a[1])) {	       \
    } else if (*a == '"' && isalpha(a[1])) {   \
    } else if (*a == '`' && isalpha(a[1])) {   \
    } else if (*a == '\'' && isalpha(a[1])) {  \
    } else if (*a == '~' && isalpha(a[1])) {   \
    } else if (*a == ',' && isalpha(a[1])) {   \
    } else {				       \
      NewKey[rover++] = *a;                    \
    }                                          \
    a++;                                       \
  }                                            \
  NewKey[rover] = 0;                           \
  safe_free(apointer);                         \
  a = NewKey


char *SanitizedString (texutil TeXUtil,char *original) {
  char *inputstring = original;
  char *apointer; /* for the macros */
  char *copied;
  char *NewKey;
  int rover;
  int PrevChar;
  if (SortN) {
    copied = safe_strdup(inputstring);
    my_concat3(&inputstring,inputstring,"\\x00",copied); /* sneaking :-) */
  } else if (ProcessQuotes) {
    DROP_ACCENT_BACKSLASHES(inputstring);
    copied = safe_strdup(inputstring);
    DROP_ACCENTED_CHARS(copied);
    DROP_ACCENTS(inputstring);
    my_concat3(&inputstring,inputstring,"\\x00",copied); /* sneaking :-) */
  }
  /* inputstring =~ s/\<\*(.*?)\>/\\$1 /go ; */
  NewKey = safe_malloc(strlen(inputstring)+1);
  apointer = inputstring;
  rover= 0;
  while (*inputstring) {
    if (*inputstring == '<') {
      inputstring++;
      if (*inputstring == '*') {
	NewKey[rover++] = '\\';
	while(*inputstring!='>') {
	  NewKey[rover++] = *inputstring++;
	}
	NewKey[rover++] = ' ';
      } else {
	NewKey[rover++] = '<';
      }
    } else {
      NewKey[rover++] = *inputstring;
      inputstring++;
    }
  }
  NewKey[rover] = 0;
  safe_free(apointer);
  inputstring = NewKey;
  /* this one will be fixed below: inputstring =~ s/\\getXMLentity\s*\{(.*?)\}/$1/gio ;*/

  /*   inputstring =~ s/\<[a-zA-Z\/].*?\>//go ; */
  NewKey = safe_malloc(strlen(inputstring)+1);
  apointer = inputstring;
  rover= 0;
  while (*inputstring) {
    if (*inputstring == '<') {
      inputstring++;
      if (*inputstring == '/' || isalpha(*inputstring)) {
	while(*inputstring!='>') {
	  NewKey[rover++] = *inputstring++;
	}
      } else {
	NewKey[rover++] = '<';
      }
    } else {
      NewKey[rover++] = *inputstring;
      inputstring++;
    }
  }
  NewKey[rover] = 0;
  safe_free(apointer);
  inputstring = NewKey;
  MAKE_DASHES(inputstring);
  /* inputstring =~ s!\\[a-zA-Z]*| |\{|\}!!gio ; */
  NewKey = safe_malloc(strlen(inputstring)+1);
  apointer = inputstring;
  rover= 0;
  while (*inputstring) {
    if (*inputstring == '\\') {
      inputstring++;
      while(isalpha(*inputstring))
	inputstring++;
    } else if (*inputstring == ' ' || *inputstring == '{' || *inputstring  == '}') {
      inputstring++;
    } else {
      NewKey[rover++] = *inputstring;
      inputstring++;
    }
  }
  NewKey[rover] = 0;
  safe_free(apointer);
  inputstring = NewKey;
  return inputstring;
}


char *HighConverted (char *inputstring) {
  /* $string =~ s/\^\^([a-f0-9][a-f0-9])/chr hex($1)/geo ;*/
  char *Class;
  char *result = NULL;
  char val[2];
  val[1] = 0;
  unsigned int *hexchar=0;
  while((Class = next_word(&inputstring, "^^"))!=NULL) {
    my_concat(&result,result,Class);
    if (ishex(inputstring[0]) && ishex(inputstring[1])) {
      if(sscanf(inputstring,"%2x",hexchar)==1) {
	val[0] = *hexchar;
	my_concat (&result,result,val);
      }
    } else {
      my_concat(&result,result,"^^");
    }
  }
  my_concat(&result,result,inputstring);
  safe_free(val);
  return result;
}


void HandleRegister (texutil TeXUtil, char *RestOfLine) {
  char *SortMethod = "";
  int SecondTag;
  if (strlen(RestOfLine) > 2 && RestOfLine[1] == ' ') {
    SecondTag = RestOfLine[0];
    RestOfLine += 2;
  } else {
    return;
  }
  char *Class=NULL;
  char *Location=NULL;
  char *Key=NULL;
  char *Entry=NULL;
  char *SeeToo=NULL;
  char *PageStr=NULL;
  char *RealPageStr=NULL;
  int Page;
  int RealPage;
  /* 'official' texutil uses "\}\s\{", but i assume there cannot be \t chars here */
  if (SecondTag == 's') {
    Class       = next_word(&RestOfLine, "} {");
    Location    = next_word(&RestOfLine, "} {");
    Key         = next_word(&RestOfLine, "} {");
    Entry       = next_word(&RestOfLine, "} {");
    SeeToo      = next_word(&RestOfLine, "} {");
    PageStr     = chomp(RestOfLine);
    if (!(Class && Location &&  Key && Entry && SeeToo && PageStr))
      return;
    Page        = atoi (PageStr);
    Class      += 1 ;
    RealPage    = 0 ;
  } else {
    Class       = next_word(&RestOfLine, "} {");
    Location    = next_word(&RestOfLine, "} {");
    Key         = next_word(&RestOfLine, "} {");
    Entry       = next_word(&RestOfLine, "} {");
    PageStr     = next_word(&RestOfLine, "} {");
    RealPageStr = chomp(RestOfLine);
    if (!(Class && Location &&  Key && Entry && PageStr && RealPageStr))
      return;
    Page        = atoi(PageStr);
    RealPage    = atoi(RealPageStr);
    Class      += 1 ; /* skips backslash */
    SeeToo      = "" ;
  }
  char *PageHow = "";
  if (strstr(Key,RegSep) != NULL)
    PageHow = next_word(&Key,RegSep);
  char *TextHow = "";
  if (strstr(Entry,RegSep) != NULL)
    TextHow = next_word(&Entry,RegSep);
  if (strlen(Key) == 0)
    Key = SanitizedString(TeXUtil,Entry);
  if (strlen(SortMethod))
    ProcessHigh = 0;
  if (ProcessHigh)
    Key = HighConverted(Key);
  int KeyTag;
  KeyTag = Key[0];
  char *NewKey;
  char *apointer;
  int rover;
  if (KeyTag == '&') {
    while (strchr(Key,'&') == 0)
      Key++;
    ESCAPED_CHAR(Key,'&');
  } else if (KeyTag == '+') {
    while (strchr(Key,'+') == 0)
      Key++;
    ESCAPED_CHAR(Key,'+');
  } else {
    NewKey = safe_strdup(Key);
    Key = NewKey;
    ESCAPED_CHAR(Key,'&');
    ESCAPED_CHAR(Key,'+');
  }
  my_concat (&Key,Key," "); /* so, "Word" comes for "Word Another Word" */
  int EntryTag;
  EntryTag = Entry[0] ;
  if (EntryTag == '&') {
    while (strchr(Entry,'&') == 0)
      Entry++;
    ESCAPED_CHAR(Entry,'&');
  } else if (EntryTag == '+') {
    while (strchr(Entry,'+') == 0)
      Entry++;
    ESCAPED_CHAR(Entry,'+');
  } else if (KeyTag == '&') {
    ESCAPED_CHAR(Entry,'&');
  } else if (KeyTag == '+') {
    ESCAPED_CHAR(Entry,'+');
  } else {
    ESCAPED_CHAR(Entry,'&');
    ESCAPED_CHAR(Entry,'+');
  }
  if (!isalpha(Key[0]))
    my_concat(&Key," ",Key);
  STRIP_PRE_SPACE_AND_BRACES(Key);
  if (*Entry == '{')
    STRIP_PRE_SPACE_AND_BRACES(Entry);
  if (ProcessIJ) {
    ESCAPED_IJ(Key);
  }
  char *LCKey = safe_strdup(Key);
  LCKey = lc(LCKey);
  int RegStatus=0;
  /* from %RegStat */
  if (SecondTag == 'f') {
    RegStatus = RegStat_f;
  } else if (SecondTag == 'e' ) {
    RegStatus = RegStat_e;
  } else if (SecondTag == 't' ) {
    RegStatus = RegStat_t;
  } else if (SecondTag == 's' ) {
    RegStatus = RegStat_s;
  }
  EXTEND_ARRAY(RegisterEntry,NOfEntries);
  char *result = safe_malloc(BUFSIZE);
  snprintf(result,BUFSIZE,
	   "%s__%s__%s__%s__%s__%i__%6i__%s__%s__%s__%s",
	   Class,LCKey,Key,Entry,TextHow,RegStatus,RealPage,Location,PageStr,PageHow,SeeToo);
  result = safe_realloc(result,strlen(result)+1);
  RegisterEntry[NOfEntries++] = result;
  safe_free(Key);
}

char *How (char *TextHow,char *in){
  char *HowJ=NULL;
  my_concat3(&HowJ,TextHow,RegSep,in);
  return HowJ;
}


#define  FlushSavedLine() {				\
  if ((CollapseEntries)&&(strlen(SavedFrom) !=0 )) {    \
    if (strlen(SavedTo) != 0) {                         \
      fprintf (TUO,"\\registerfrom%s", SavedFrom) ;     \
      fprintf (TUO,"\\registerto%s" , SavedTo) ;        \
    } else {                                            \
      fprintf (TUO,"\\registerpage%s",SavedFrom);       \
    } }                                                 \
    SavedFrom  = "" ;                                   \
    SavedTo    = "" ;                                   \
    SavedEntry = "" ;                                   \
}

void FlushRegisters (texutil TeXUtil) {
  int CollapseEntries=0; /*useless variable */
  fprintf (TUO, "%%\n%% %s / Registers\n%%\n",Program );
  qsort(RegisterEntry,NOfEntries,sizeof(char *),my_strcasecmp);
  int NOfSaneEntries = 0 ;
  int NOfSanePages   = 0 ;
  char *LastPage     = "";
  int LastRealPage = 0;
  char *AlfaClass      = NULL;
  char Alfa = 0;
  char *PreviousA      = "" ;
  char *PreviousB      = "" ;
  char *PreviousC      = "" ;
  char *ActualA        = "" ;
  char *ActualB        = "" ;
  char *ActualC        = "" ;
  char *SavedFrom  = "" ;
  char *SavedTo    = "" ;
  char *SavedEntry = "" ;
  int n;
  char *tempentry;
  char *SavedLine = safe_malloc(BUFSIZE);
  char *NextEntry = safe_malloc(BUFSIZE);
  int Copied = 0;
  char TestAlfa = 0;
  for (n=0 ; n<NOfEntries ; n++) {
    tempentry = RegisterEntry[n];
    char *Class         = next_word(&tempentry,JOIN);
    char *LCKey         = next_word(&tempentry,JOIN);
    LCKey = NULL; /* not used */
    char *Key           = next_word(&tempentry,JOIN);
    char *Entry         = next_word(&tempentry,JOIN);
    char *TextHow       = next_word(&tempentry,JOIN);
    int RegisterState   = atoi(next_word(&tempentry,JOIN));
    char *RealPageStr   = next_word(&tempentry,JOIN);
    char *Location      = next_word(&tempentry,JOIN);
    char *Page          = next_word(&tempentry,JOIN);
    char *PageHow       = next_word(&tempentry,JOIN);
    char *SeeToo        = tempentry;
    while (isspace(*RealPageStr))
      RealPageStr++;
    int RealPage = atoi (RealPageStr);
    TestAlfa = tolower(Key[0]);
    if (SortN){
      /*
	$AlfKey = $Key ;
	$AlfKey =~ s!(.).*\x00(.).*!$2!o ;
	if (defined($ALF{$AlfKey}))
	  TestAlfa = $ALF{$AlfKey} ;
      */
    }
    if (TestAlfa != Alfa || (!strcmp(AlfaClass,Class)==0)) {
      Alfa = TestAlfa ;
      safe_free(AlfaClass);
      AlfaClass = safe_strdup(Class);
      if (Alfa != ' ')  {
	FlushSavedLine();
	fprintf (TUO, "\\registerentry{%s}{%c}\n", Class,Alfa);
      }
    }
    tempentry = Entry;
    if (strstr(tempentry,SPLIT) != NULL) {
      ActualA = next_word(&tempentry,SPLIT);
      if (strstr(tempentry,SPLIT) != NULL) {
	ActualB = next_word(&tempentry,SPLIT);
	ActualC = tempentry;
      } else {
	ActualB = tempentry;
	ActualC = "";
      }
    } else {
      ActualA = tempentry;
      ActualB = "";
      ActualC = "";
    }
    char *HowJ = How(TextHow,ActualA);
    if (STREQ(HowJ,PreviousA)) {
      safe_free(HowJ);
      ActualA = "" ;
    } else {
      PreviousA = HowJ;
      PreviousB = "" ;
      PreviousC = "" ;
    }
    HowJ = How(TextHow,ActualB);
    if (STREQ(HowJ,PreviousB)) {
      ActualB = "" ;
      safe_free(HowJ);
    } else {
      PreviousB = HowJ ;
      PreviousC = "" ;
    }
    HowJ = How(TextHow,ActualC);
    if (STREQ(HowJ,PreviousC)) {
      ActualC = "";
      safe_free(HowJ);
    } else {
      PreviousC = HowJ ;
    }
    Copied = 0 ;
    if (strlen(ActualA)) {
      FlushSavedLine() ;
      fprintf (TUO, "\\registerentrya{%s}{%s}\n" , Class,ActualA);
      Copied = 1;
    }
    if (strlen(ActualB)) {
      FlushSavedLine() ;
      fprintf (TUO, "\\registerentryb{%s}{%s}\n", Class, ActualB);
      Copied = 1 ;
    }
    if (strlen(ActualC)) {
      FlushSavedLine() ;
      fprintf (TUO, "\\registerentryc{%s}{%s}\n" , Class, ActualC);
      Copied = 1;
    }
    if (Copied)
      NOfSaneEntries++;
    if (RealPage == 0) {
      FlushSavedLine() ;
      fprintf (TUO,"\\registersee{%s}{%s,%s}{%s}{%s}\n",Class,PageHow,TextHow,SeeToo,Page) ;
      LastPage = Page ;
      LastRealPage = RealPage;
    } else if ((Copied) || ! ((STREQ(LastPage,Page)) && (LastRealPage == RealPage))) {
      snprintf(NextEntry,BUFSIZE,"{%s}{%s}{%s}{%s}{%s,%s}",Class,PreviousA,PreviousB,PreviousC,PageHow,TextHow);
      snprintf(SavedLine,BUFSIZE,"{%s}{%s,%s}{%s}{%s}{%d}\n",Class,PageHow,TextHow,Location,Page,RealPage);
      if (RegisterState == RegStat_f)  {
	FlushSavedLine() ;
	fprintf( TUO, "\\registerfrom%s",SavedLine);
      } else if (RegisterState == RegStat_t) {
	FlushSavedLine() ;
	fprintf (TUO, "\\registerto%s" , SavedLine);
      } else {
	if (CollapseEntries) {
	  if (!STREQ(SavedEntry,NextEntry)) {
	    SavedFrom = SavedLine;
	  } else {
	    SavedTo = SavedLine;
	  }
	  SavedEntry = NextEntry;
	} else {
	  fprintf (TUO, "\\registerpage%s", SavedLine);
	}
      }
      ++NOfSanePages ;
      LastPage = Page ;
      LastRealPage = RealPage ;
    }
    safe_free(RegisterEntry[n]);
  }
  safe_free(AlfaClass);
  FlushSavedLine();
  safe_free(NextEntry);
  safe_free(SavedLine);
  safe_free(RegisterEntry);
  fprintf(STDOUT,"%s %d -> %d %s %d %s\n",MS[ID_RegisterEntries],NOfEntries,
	  NOfSaneEntries,MS[ID_Entries],NOfSanePages,MS[ID_References]);
  if (NOfBadEntries>0)
    fprintf(STDOUT,"%s %d\n",MS[ID_RegisterErrors],NOfBadEntries);
}


void HandleSynonym (texutil TeXUtil, char *RestOfLine)  {
  char *index;
  index = strstr(RestOfLine," ");
  if (index) {
    /* SecondTag not used */
    RestOfLine = ++index;
  }
  char *myline = RestOfLine;
  char *Class;
  char *Key;
  char *Entry = "";
  char *Meaning = "";
  Class = next_word(&myline,"} {");
  Key   = next_word(&myline,"} {");
  Entry = next_word(&myline,"} {");
  Meaning = myline;
  Meaning = chop(Meaning);
  Class += 1;
  if (!strlen(Entry)) {
    ++NOfBadSynonyms ;
  } else {
    EXTEND_ARRAY(SynonymEntry,NOfSynonyms);
    char *entry = safe_malloc(BUFSIZE);
    snprintf(entry,BUFSIZE,"%s__%s__%s__%s",Class,Key,Entry,Meaning);
    entry = safe_realloc(entry,strlen(entry)+1);
    SynonymEntry[NOfSynonyms++] = entry;
  }
}

void FlushSynonyms (texutil TeXUtil) {
  fprintf(TUO, "%%\n%% %s / Synonyms\n%%\n", Program );
  qsort(SynonymEntry,NOfSynonyms,sizeof(char *),my_strcasecmp);
  int NOfSaneSynonyms = 0 ;
  int n;
  char *tmpentry;
  char *ztmpentry;
  char *Class;
  char *Key;
  char *Entry;
  char *Meaning;
  for (n=0; n<NOfSynonyms; n++) {
    if ((n==0)||(!STREQ(SynonymEntry[n],SynonymEntry[n-1]))) {
      tmpentry = safe_strdup(SynonymEntry[n]);
      ztmpentry = tmpentry;
      Class="";
      Key="";
      Entry="";
      Meaning="";
      Class = next_word(&tmpentry,JOIN);
      Key   = next_word(&tmpentry,JOIN);
      Entry = next_word(&tmpentry,JOIN);
      Meaning = tmpentry;
      ++NOfSaneSynonyms ;
      fprintf (TUO, "\\synonymentry{%s}{%s}{%s}{%s}\n", Class,Key,Entry,Meaning);
      safe_free (ztmpentry);
      if (n!=0)
	safe_free (SynonymEntry[n-1]);
    }
  }
  if(NOfSynonyms)
    safe_free(SynonymEntry[NOfSynonyms]);
  safe_free(SynonymEntry);
  fprintf(STDOUT,"%s %d -> %d %s\n",
	  MS[ID_SynonymEntries],NOfSynonyms,NOfSaneSynonyms,MS[ID_Entries]);
  if (NOfBadSynonyms>0)
    fprintf(STDOUT,"%s %d\n",MS[ID_SynonymErrors],NOfBadSynonyms);
}


void HandleFile (texutil TeXUtil,char *RestOfLine) {
  char *kick;
  RestOfLine = strchr(RestOfLine,'{');
  if (RestOfLine == NULL)
    return;
  RestOfLine++;
  kick = strrchr(RestOfLine,'}');
  if (kick == NULL)
    return;
  *kick = 0;
  int test = -1;
  int i;
  for (i=0;i<=NOfFiles;i++) {
    if(STREQ(Files[i],RestOfLine))
      test = i;
  }
  if (test>=0) {
    File_opens[test]++;
  } else {
    if (Files_size < NOfFiles) {
      Files = safe_realloc(Files,sizeof(char*)*(Files_size+Files_add));
      File_opens = safe_realloc(File_opens,sizeof(int)*(Files_size+Files_add));
      Files_size = Files_size+Files_add;
    }
    File_opens[NOfFiles] = 1;
    Files[NOfFiles] = safe_strdup(RestOfLine);
    NOfFiles++;
  }
}


void FlushFiles (texutil TeXUtil) {
  int i;
  fprintf (TUO, "%%\n%% %s / Files\n%%\n" , Program);
  for (i=0; i<NOfFiles; i++) {
    fprintf (TUO, "%% %s (%d)\n" , Files[i],File_opens[i]);
  }
  fprintf (TUO,"%%\n" );
  fprintf (STDOUT,"%s %d\n", MS[ID_EmbeddedFiles],NOfFiles);
  for (i=0; i<NOfFiles; i++) {
    if ((File_opens[i] % 2) != 0) {
      ++NOfBadFiles ;
      fprintf (STDOUT,"%s %s\n", MS[ID_BeginEndError],Files[i]);
    }
    safe_free(Files[i]);
  }
  safe_free(File_opens);
  safe_free(Files);
}

void FlushData (texutil TeXUtil)  {
  fprintf (TUO, "%%\n%% %s / Status\n%%\n" , Program);
  fprintf (TUO, "%% embedded files    : %d (%d errors)\n",NOfFiles,NOfBadFiles);
  fprintf (TUO, "%% synonym entries   : %d (%d errors)\n",NOfSynonyms,NOfBadSynonyms);
  fprintf (TUO, "%% register entries  : %d (%d errors)\n",NOfEntries,NOfBadEntries);
  fprintf (TUO, "%% metapost graphics : %d\n",TotalNOfMPgraphics);
  fprintf (TUO, "%% position commands : %d ", TotalNOfPositions);
  if (TotalNOfPositions) {
    if (NOfPositionsFound) {
      fprintf (TUO, "(resolved)\n");
    } else {
      fprintf( TUO, "(unresolved)\n");
    }
  } else {
    fprintf (TUO, "(not used)\n" );
  }
}

void MergerHandleReferences (texutil TeXUtil,int ValidOutput,string *ARGV) {
  char *Suffix ;
  char *TempInputFile;
  unlink("texutil.tuo");
  fprintf(STDOUT,"%s %s\n",MS[ID_OutputFile],"texutil.tuo" ) ;
  safe_fopen (TUO,"texutil.tuo","w") ;
  while (*ARGV) {
    TempInputFile = *ARGV++;
    InputFile = next_word(&TempInputFile,".");
    Suffix = TempInputFile;
    char *tuifile = NULL;
    my_concat(&tuifile,InputFile,".tui");
    FILE *TUI;
    safe_fopen(TUI,tuifile,"r");
    safe_free(tuifile);
    if(!TUI) {
      fprintf(STDOUT,"%s %s %s\n",MS[ID_Error], MS[ID_EmptyInputFile], InputFile) ;
    } else {
      fprintf(STDOUT,"%s %s.tui\n", MS[ID_InputFile], InputFile) ;
      char *SomeLine = safe_malloc(BUFSIZE);
      while (readline(TUI,&SomeLine)) {
	SomeLine = chomp (SomeLine);
	if (SomeLine[0] == 'r' && SomeLine[1] == ' ')
	  HandleRegister(TeXUtil,(SomeLine+2));
      }
      safe_free(SomeLine);
      safe_fclose(TUI);
    }
  }
  if (ValidOutput) {
    FlushRegisters(TeXUtil) ;
    safe_fclose (TUO);
  } else {
    safe_fclose (TUO) ;
    unlink ("texutil.tuo");
    fprintf(STDOUT,"%s %s\n",MS[ID_Remark],MS[ID_NoOutputFile]);
  }
}

void NormalHandleReferences (texutil TeXUtil, int ValidOutput) {
  if (!strlen(InputFile)) {
    fprintf(STDOUT,"%s %s\n",MS[ID_Error], MS[ID_NoInputFile]);
  } else {
    char *tuifile = NULL;
    my_concat(&tuifile,InputFile,".tui");
    FILE *TUI;
    safe_fopen(TUI,tuifile,"r");
    if (!TUI) {
      safe_free(tuifile);
      fprintf(STDOUT,"%s %s %s\n",MS[ID_Error], MS[ID_EmptyInputFile], InputFile);
    } else {
      fprintf(STDOUT,"%s %s\n",MS[ID_InputFile], tuifile);
      safe_free(tuifile);
      char *tmpfile = NULL;
      char *tuofile = NULL;
      my_concat(&tuofile,InputFile,".tuo");
      my_concat(&tmpfile,InputFile,".tmp");
      unlink (tmpfile);
      rename (tuofile, tmpfile);
      fprintf(STDOUT,"%s %s\n",MS[ID_OutputFile], tuofile);
      safe_fopen (TUO,tuofile,"w");
      if (!TUO) {
	safe_free(tmpfile);
	safe_free(tuofile);
	return;
      }
      fprintf (TUO,"%%\n%% %s / Commands\n%%\n",Program);
      char *SomeLine = safe_malloc(BUFSIZE);
      char *RestOfLine=NULL;
      char FirstTag;
      while (readline(TUI,&SomeLine)) {
	SomeLine = chomp (SomeLine);
	FirstTag = SomeLine[0];
	if (strlen(SomeLine) > 2 && SomeLine[1] == ' ') {
	  RestOfLine = SomeLine+2;
	} else {
	  FirstTag = 'x';
	}
	if    (FirstTag == 'c' ) {
	  HandleCommand(TeXUtil,RestOfLine);
	} else if (FirstTag == 's') {
	  HandleSynonym(TeXUtil,RestOfLine);
	} else if (FirstTag == 'r') {
	  HandleRegister(TeXUtil,RestOfLine) ;
	} else if (FirstTag == 'f') {
	  HandleFile(TeXUtil,RestOfLine);
	} else if (FirstTag == 'k') {
	  /* HandleKey(RestOfLine); */
	  fprintf(STDOUT,"%s (HandleKey) %s\n",MS[ID_Error],MS[ID_NotYetImplemented]);
	} else if (FirstTag == 'e') {
	  HandleExtra(TeXUtil,RestOfLine);
	} else if (FirstTag == 'p') {
	  fprintf(STDOUT,"%s (HandlePlugIn) %s\n",MS[ID_Error],MS[ID_NotYetImplemented]);
	  /* HandlePlugIn(RestOfLine); */
	} else if (FirstTag == 'q') {
	  ValidOutput = 0 ;
	  goto EXIT;
	}
      }
    EXIT:
      safe_free(SomeLine);
      if (ValidOutput) {
	FlushCommands(TeXUtil) ;
	FlushKeys(TeXUtil) ;
	FlushRegisters(TeXUtil) ;
	FlushSynonyms(TeXUtil) ;
	FlushFiles(TeXUtil) ;
	FlushData(TeXUtil) ;
	FlushExtra(TeXUtil) ;
	safe_fclose (TUO) ;
	RunExtraPrograms(TeXUtil);
      } else {
	safe_fclose (TUO) ;
	unlink (tuofile);
	rename (tmpfile, tuofile);
	fprintf(STDOUT,"%s %s\n", MS[ID_Remark], MS[ID_NoOutputFile]);
      }
      safe_free(tuofile);
      safe_free(tmpfile);
    }
  }
}


void HandleReferences (texutil TeXUtil, int ARGC,string *ARGV) {
    if (ARGC>1) {
      fprintf(STDOUT,"%s %s\n",MS[ID_Action],MS[ID_MergingReferences]);
    } else if (ARGC){
      fprintf(STDOUT,"%s %s\n",MS[ID_Action],MS[ID_ProcessingReferences]);
    } else {
      return;
    }
    if (ProcessIJ)
      fprintf(STDOUT,"%s %s\n",MS[ID_Option],MS[ID_SortingIJ]);
    if (ProcessHigh)
      fprintf(STDOUT,"%s %s\n",MS[ID_Option],MS[ID_ConvertingHigh]);
    if (ProcessQuotes)
      fprintf(STDOUT,"%s %s\n",MS[ID_Option],MS[ID_ProcessingQuotes]);
    InitializeKeys(TeXUtil, ARGV) ;
    ExtraPrograms = safe_malloc(sizeof(char*)*ExtraPrograms_size);
    RegisterEntry = safe_malloc(sizeof(char *)*RegisterEntry_size);
    SynonymEntry  = safe_malloc(sizeof(char *)*SynonymEntry_size);
    Files         = safe_malloc(sizeof(char *)*Files_size);
    File_opens    = safe_malloc(sizeof(int)*Files_size);
    int ValidOutput = 1 ;
    if (ARGC>1)  {
      MergerHandleReferences (TeXUtil, ValidOutput,ARGV);
    } else {
      NormalHandleReferences (TeXUtil, ValidOutput);
    }
}


void SaveFigurePresets (texutil TeXUtil, char *FNam, char *FTyp, int FUni, double FXof, double FYof,
		   double FWid, double FHei, char *FTit, char *FCre, int FSiz) {
  char *line = NULL;
  if (ProcessVerbose) {
    OpenTerminal(TeXUtil);
    fprintf (STDOUT,"n=%s t=%s " , FNam, FTyp);
    if (FUni) {
      fprintf (STDOUT,"x=%1.3fcm y=%1.3fcm ", FXof, FYof);
      fprintf (STDOUT,"w=%5.3fcm h=%5.3fcm\n", FWid, FHei);
    } else  {
      fprintf (STDOUT,"x=%dbp y=%dbp ",  (int)FXof, (int)FYof);
      fprintf (STDOUT,"w=%dbp h=%dbp\n", (int)FWid, (int)FHei);
    }
    CloseTerminal(TeXUtil);
  } else {
    line = safe_malloc(BUFSIZE);
    char *FCreString = NULL;
    if (strlen(FCre))
      my_concat3(&FCreString,",c={",FCre,"}");
    char *FTitString = NULL;
    if (strlen(FTit))
      my_concat3(&FTitString,",t={",FTit,"}");
    char *WHstring = safe_malloc(30);
    if (FUni) {
      snprintf (WHstring,30,"w=%5.3fcm,h=%5.3fcm", FWid, FHei);
    } else {
      snprintf (WHstring,30,"w=%dbp,h=%dbp", (int)FWid, (int)FHei);
    }
    char *Offstring = safe_malloc(30);
    if ((FXof!=0.0)||(FYof!=0.0)) {
      if (FUni) {
	snprintf (Offstring,30,",x=%1.3fcm,y=%1.3fcm", FXof, FYof);
      } else {
	snprintf (Offstring,30,",x=%dbp,y=%dbp", (int)FXof, (int)FYof);
      }
    } else {
      Offstring = NULL;
    }
    snprintf(line, BUFSIZE, "\\presetfigure[%s][e=%s,%s%s%s%s,s=%d]\n",
	     FNam, FTyp, WHstring,(Offstring?Offstring:""),(FCreString?FCreString:""),
	     (FTitString?FTitString:""),FSiz);
    safe_free(Offstring);
    safe_free(WHstring);
    safe_free(FCreString);
    safe_free(FTitString);
    EXTEND_ARRAY(Figures,NOfFigures);
    Figures[NOfFigures++] = line;
  }
}

void ConvertEpsToEps (texutil TeXUtil, char *SName, int PDFReady, 
		      double LLX, double LLY, double URX, double URY) {
  char *FileName ;
  char *FileSuffix; /* not used */
  FileName = safe_strdup(SName);
  FileSuffix = SplitFileName (&FileName) ;
  FILE *EPS;
  FILE *TMP;
  if (ProcessEpsToPdf) {
    char *pdffile = NULL;
    my_concat(&pdffile,FileName,".pdf");
    unlink (pdffile);
    char cmdline[BUFSIZE];
    snprintf(cmdline,BUFSIZE,
	     "%s -q -sDEVICE=pdfwrite -dNOCACHE "
	     "-dUseFlateCompression=true -dMaxSubsetPct=100 -sOutputFile="
	     "%s - -c quit", gs, pdffile);
    EPS = popen(cmdline,"w");
  } else if (PDFReady) {
    return ;
  } else {
    safe_fopen (EPS,"texutil.tmp" ,"wb") ;
  }
  safe_fopen (TMP,SName,"rb") ;
  int EpsBBOX = 0 ;
  double EpsWidth   = URX - LLX ;
  double EpsHeight  = URY - LLY ;
  double EpsXOffset =   0 - LLX ;
  double EpsYOffset =   0 - LLY ;
  char *inputline = safe_malloc(BUFSIZE);
  while (readline(TMP,&inputline))  {
    if (strstr(inputline,"%!PS")!= NULL){
      inputline = strstr(inputline,"%!PS");
      fprintf (EPS,"%s", inputline );
      goto DONE;
    }
  }
 DONE:
  while (readline(TMP, &inputline)) {
    if ((!PDFReady)&&
	((strncmp(inputline,"%%BoundingBox:",     strlen("%%BoundingBox:"))==0)||
	 (strncmp(inputline,"%%HiResBoundingBox:",strlen("%%HiResBoundingBox:"))==0)||
	 (strncmp(inputline,"%%ExactBoundingBox:",strlen("%%ExactBoundingBox:"))==0))) {
      if (!EpsBBOX) {
	fprintf (EPS, "%%%%PDFready: %s\n" , Program);
	fprintf (EPS, "%%%%BoundingBox: 0 0 %d %d\n", (int)EpsWidth, (int)EpsHeight);
	fprintf (EPS, "<< /PageSize [%d %d] >> setpagedevice\n", (int)EpsWidth, (int)EpsHeight);
	fprintf (EPS, "gsave %d %d translate\n",(int)EpsXOffset , (int)EpsYOffset );
	EpsBBOX = 1;
      }
    } else if (strncmp(inputline,"%%EOF",strlen("%%EOF"))==0) {
      goto DONE2;
    } else if (strncmp(inputline,"%%Trailer",strlen("%%Trailer"))==0) {
      goto DONE2;
    } else {
      fprintf (EPS, "%s", inputline) ;
    }
  }
 DONE2:
  safe_fclose ( TMP ) ;
  if ((EpsBBOX)&&(!PDFReady)) {
    fprintf (EPS, "grestore\n%%%%EOF\n%%%%RestOfFileIgnored: %s\n", Program) ;
    if (ProcessEpsToPdf) {
      pclose ( EPS ) ;
    } else {
      safe_fclose ( EPS ) ;
    }
    fprintf (STDOUT, "%s %s\n",MS[ID_PdfFile], SName);
    if (!ProcessEpsToPdf) {
      unlink (SName) ;
      rename ("texutil.tmp", SName);
    }
  } else {
    if (ProcessEpsToPdf) {
      pclose ( EPS ) ;
    } else {
      safe_fclose ( EPS ) ;
    }
  }
  unlink ("texutil.tmp");
 }


#define SET_CORNER(Box,a)     \
  while (isspace(*Box))       \
      Box++;                  \
    while (isdigit(*Box) ||   \
           *Box == '.' ||     \
	   *Box == '-')       \
      *rover++ = *Box++;      \
    *rover = 0;               \
    rover = value;            \
    a = strtod(value,NULL)


void HandleEpsFigure (texutil TeXUtil,char *SuppliedFileName) {
  char *Temp = "" ;
  char *FileName;
  char *FileSuffix;
  char *EpsFileName;
  if (file_exists(SuppliedFileName)) {
    FileName = safe_strdup(SuppliedFileName);
    char *filenamepointer  = FileName;
    FileSuffix = SplitFileName (&FileName) ;
    if (FileSuffix && strlen(FileSuffix)) {
      int matchesdigits = 0;
      if (isdigit(FileSuffix[0])) {
	Temp = FileSuffix;
	while (isdigit(*Temp))
	  Temp++;
	if (*Temp==0)
	  matchesdigits = 1;
      }
      if (matchesdigits) {
	EpsFileName = SuppliedFileName;
	fprintf (STDOUT,"%s %s\n",MS[ID_MPFile], SuppliedFileName );
      } else if (!(strcasecmp(FileSuffix,"eps")==0)
		   &&
		 !(strcasecmp(FileSuffix,"mps")==0)) {
	safe_free(filenamepointer);
	return ;
      } else {
	EpsFileName = SuppliedFileName;
	fprintf (STDOUT,"%s %s\n",MS[ID_EpsFile], SuppliedFileName );
      }
      safe_free(filenamepointer);
      char *EpsTitle = "";
      char *EpsCreator = "" ;
      FILE *EPS;
      safe_fopen (EPS,SuppliedFileName,"rb") ;
      if (!EPS)
	return;
      long int EpsSize = 0;
      if(!fseek(EPS,0L,SEEK_END)) {
	EpsSize = ftell(EPS);
	rewind(EPS);
      } else {
	safe_fclose(EPS) ;
	return;
      }
      int PDFReady = 0 ;
      int MPSFound = 0 ;
      int BBoxFound = 0 ;
      char *inputline = safe_malloc(BUFSIZE);
      char *SomeLine;
      char *EpsBBox=NULL;
    RESTART:
      while (readline(EPS,&inputline)) {
	SomeLine = chomp(inputline);
	if (!strlen(SomeLine))
	  goto RESTART;
	if ((BBoxFound) && (SomeLine[0] != '%')) {
	  goto END;
	}
	if (BBoxFound<2) {
	  if (strncasecmp(SomeLine,"%%BoundingBox:",strlen("%%BoundingBox:"))==0) {
	    EpsBBox = safe_strdup(SomeLine+strlen("%%BoundingBox:")) ; BBoxFound = 1 ; goto RESTART ;
	  } else if (strncasecmp(SomeLine,"%%HiResBoundingBox:",strlen("%%HiResBoundingBox:"))==0) {
	    EpsBBox = safe_strdup(SomeLine+strlen("%%HiResBoundingBox:")) ; BBoxFound = 2 ; goto RESTART ;
	  } else if (strncasecmp(SomeLine,"%%ExactBoundingBox:",strlen("%%ExactBoundingBox:"))==0) {
	    EpsBBox = safe_strdup(SomeLine+strlen("%%ExactBoundingBox:")) ; BBoxFound = 3 ; goto RESTART ;
	  }
	}
	if (strncasecmp(SomeLine,"%%PDFready:",strlen("%%PDFready:"))==0) {
	  PDFReady = 1 ;
	} else if (strncasecmp(SomeLine,"%%Creator:",strlen("%%Creator:"))==0) {
	  SomeLine += strlen("%%Creator:");
	  while (isspace(*SomeLine))
	    SomeLine++;
	  EpsCreator = safe_strdup(SomeLine);
	  if (strncasecmp(EpsCreator,"MetaPost",strlen("MetaPost"))==0) {
	    MPSFound = 1 ;
	  }
	} else if (strncasecmp(SomeLine,"%%Title:",strlen("%%Title:"))==0) {
	  SomeLine += strlen("%%Title:");
	  while (isspace(*SomeLine))
	    SomeLine++;
	  EpsTitle = safe_strdup(SomeLine);
	}
      }
    END:
      safe_free(inputline);
      safe_fclose ( EPS ) ;
      if (BBoxFound) {
	double URX=0.0;
	double URY=0.0;
	double LLX=0.0;
	double LLY=0.0;
	char *value=safe_malloc(strlen(EpsBBox));
	char *BBoxpointer = EpsBBox;
	char *rover = value;
	SET_CORNER(EpsBBox,LLX);
	SET_CORNER(EpsBBox,LLY);
	SET_CORNER(EpsBBox,URX);
	SET_CORNER(EpsBBox,URY);
	safe_free(BBoxpointer);
	safe_free(value);
	double EpsHeight  = (URY-LLY)*DPtoCM ;
	double EpsWidth   = (URX-LLX)*DPtoCM ;
	double EpsXOffset = LLX*DPtoCM ;
	double EpsYOffset = LLY*DPtoCM ;
	char *EpsType;
	char *Epstype_eps = "eps";
	char *Epstype_mps = "mps";
	if (MPSFound) {
	  EpsType = Epstype_mps;
	} else {
	  EpsType = Epstype_eps;
	}
	SaveFigurePresets ( TeXUtil, EpsFileName, EpsType, 1,
			    EpsXOffset, EpsYOffset, EpsWidth, EpsHeight,
			    EpsTitle, EpsCreator, EpsSize ) ;
	if (strlen(EpsTitle))
	  safe_free(EpsTitle);
	if (strlen(EpsCreator))
	  safe_free(EpsCreator);
	if (ProcessEpsPage||ProcessEpsToPdf)
	  ConvertEpsToEps (TeXUtil, SuppliedFileName, PDFReady, LLX, LLY, URX, URY );
      } else {
	fprintf (STDOUT,"%s %s\n",MS[ID_MissingBoundingBox], SuppliedFileName );
      }
    }
  }
}


void HandlePdfFigure (texutil TeXUtil,char *SuppliedFileName) {
  char *FileName;
  char *FileSuffix;
  int MediaBoxFound = 0 ;
  char *MediaBox =NULL;
  int PageFound = 0 ;
  int PagesFound = 0 ;
  FileName = safe_strdup(SuppliedFileName);
  char *filenamepointer = FileName;
  FileSuffix = SplitFileName (&FileName) ;
  if (FileSuffix == NULL || strcasecmp(FileSuffix,"pdf")!= 0) {
    safe_free(filenamepointer);
    return ;
  }
  safe_free(filenamepointer);
  char *PdfFileName = SuppliedFileName ;
  fprintf(STDOUT,"%s %s\n", MS[ID_PdfFile], SuppliedFileName);
  FILE *PDF;
  safe_fopen(PDF,SuppliedFileName,"rb") ;
  if (!PDF)
    return;
  long int PdfSize = 0;
  if(!fseek(PDF,0L,SEEK_END)) {
    PdfSize = ftell(PDF);
    rewind(PDF);
  } else {
    safe_fclose (PDF) ;
    return;
  }
  char *inputline = safe_malloc(BUFSIZE);
  char *SomeLine;
  char *test;
  while (readline(PDF,&inputline)) {
    SomeLine = chomp (inputline) ;
    test = strstr(SomeLine,"/Type");
    if (test != NULL) {
      test += strlen("/Type");
      while (isspace(*test))
	test++;
      if(strstr(test,"/Pages")==test) {
	PagesFound = 1 ;
      } else if (strstr(test,"/Page")==test) {
	++PageFound ;
	if (PageFound>1) {
	  goto END;
	}
      }
    }
    if ((PageFound||PagesFound) && (strstr(SomeLine,"/MediaBox")!=NULL)) {
      MediaBox = safe_strdup((strstr(SomeLine,"/MediaBox")+strlen("/MediaBox")));
      MediaBoxFound = 1 ;
      if (PagesFound) {
	goto END;
      }
    }
  }
 END:
  safe_free(inputline);
  safe_fclose ( PDF ) ;
  if (PageFound>1)
    fprintf(STDOUT,"%s %s\n", MS[ID_MultiPagePdfFile], SuppliedFileName) ;
  if ((MediaBoxFound) && (strlen(MediaBox))) {
    double URX=0.0;
    double URY=0.0;
    double LLX=0.0;
    double LLY=0.0;
    char *value=safe_malloc(strlen(MediaBox));
    char *mediaboxpointer = MediaBox;
    char *rover = value;
    while (isspace(*MediaBox))
      MediaBox++;
    if (*MediaBox == '[')
      MediaBox++;
    SET_CORNER(MediaBox,LLX);
    SET_CORNER(MediaBox,LLY);
    SET_CORNER(MediaBox,URX);
    SET_CORNER(MediaBox,URY);
    safe_free(mediaboxpointer);
    safe_free(value);
    double PdfHeight = (URY-LLY)*DPtoCM ;
    double PdfWidth = (URX-LLX)*DPtoCM ;
    double PdfXOffset = LLX*DPtoCM ;
    double PdfYOffset = LLY*DPtoCM ;
    SaveFigurePresets (TeXUtil, PdfFileName,"pdf",1,PdfXOffset,PdfYOffset,PdfWidth,PdfHeight,"","",PdfSize);
  } else {
    fprintf(STDOUT,"%s %s\n", MS[ID_MissingMediaBox], SuppliedFileName) ;
  }
}


#define TifGetByte() getc(TIF)

int TifGetShort(FILE *TIF,int TifLittleEndian) {
  int ret;
  if(TifLittleEndian) {
    ret =getc(TIF);
    ret = ret + (getc(TIF)*256);
  } else {
    ret = getc(TIF)*256;
    ret = ret+ getc(TIF);
  }
  return ret;
}

int TifGetLong(FILE *TIF,int TifLittleEndian) {
  int ret;
  if(TifLittleEndian) {
    ret = getc(TIF);
    ret = ret + (getc(TIF)*256);
    ret = ret + (getc(TIF)*256*256);
    ret = ret + (getc(TIF)*256*256*256);
  } else {
    ret = getc(TIF)*256*256*256;
    ret = ret + (getc(TIF)*256*256);
    ret = ret + (getc(TIF)*256);
    ret = ret + getc(TIF);
  }
  return ret;
}

#define TifGetRational()     \
  ((double)TifGetLong(TIF,TifLittleEndian)/TifGetLong(TIF,TifLittleEndian))

#define TifGetAscii(TifValues,a) {  \
  char *S;                          \
  --TifValues;                      \
  if (!TifValues) {                 \
    a = NULL ;                      \
  } else {                          \
    S = safe_malloc (TifValues+1);  \
    fread (S, 1, TifValues, TIF) ;  \
    S[TifValues] = 0;               \
    a = S ;                         \
  }                                 \
}

#define TifGetWhatever_i(type,a)  \
  if (type==1) {                  \
    a=(unsigned)TifGetByte();	  \
  } else if (type==3) {           \
    a=(unsigned)TifGetShort(TIF,TifLittleEndian); \
  } else if (type==4) {           \
    a=(unsigned)TifGetLong(TIF,TifLittleEndian);  \
  } else if (type==5) {           \
    a = (unsigned)TifGetRational();		\
  } else {                        \
    a=0;			  \
  }

#define TifGetWhatever_f(type,a)  \
  if (type==1) {                  \
    a=(double)TifGetByte();       \
  } else if (type==3) {           \
    a=(double)TifGetShort(TIF,TifLittleEndian);	  \
  } else if (type==4) {           \
    a=(double)TifGetLong(TIF,TifLittleEndian);	  \
  } else if (type==5) {           \
    a = (double)TifGetRational();		\
  } else {                        \
    a=0.0;          		  \
  }

#define TifGetChunk() {				       \
  fseek (TIF, TifNextChunk, 0 );                       \
  int Length = TifGetShort(TIF,TifLittleEndian) ;      \
  TifNextChunk += 2 ;                                  \
  int i;                                               \
  for (i=1; i<=Length; i++) {                          \
    fseek (TIF, TifNextChunk, 0 );                     \
    int TifTag = TifGetShort(TIF,TifLittleEndian) ;    \
    int TifType = TifGetShort(TIF,TifLittleEndian) ;   \
    int TifValues = TifGetLong(TIF,TifLittleEndian) ;  \
    if (TifTag==256) {                                 \
      TifGetWhatever_f(TifType,TifWidth);              \
    } else if (TifTag==257) {                          \
      TifGetWhatever_f(TifType,TifHeight);             \
    } else if (TifTag==296) {                          \
      TifGetWhatever_i(TifType,TifUnit);               \
    } else if (TifTag==282) {                          \
      fseek (TIF, TifGetLong(TIF,TifLittleEndian), 0 );\
      TifGetWhatever_i(TifType,TifHRes);               \
    } else if (TifTag==283) {                          \
      fseek (TIF, TifGetLong(TIF,TifLittleEndian), 0 );\
      TifGetWhatever_i(TifType,TifVRes);               \
    } else if (TifTag==350) {                          \
      fseek (TIF, TifGetLong(TIF,TifLittleEndian), 0 );\
      TifGetAscii(TifValues,TifCreator);               \
    } else if (TifTag==315) {                          \
      fseek (TIF, TifGetLong(TIF,TifLittleEndian), 0 );\
      TifGetAscii(TifValues,TifAuthor);                \
    } else if (TifTag==269) {                          \
      fseek (TIF, TifGetLong(TIF,TifLittleEndian), 0 );\
      TifGetAscii(TifValues,TifTitle);                 \
    }                                                  \
    TifNextChunk += 12;                                \
  }                                                    \
  fseek (TIF, TifNextChunk, 0 );                       \
  TifNextChunk = TifGetLong (TIF,TifLittleEndian);     \
  if (TifNextChunk<=0)                                 \
    break ;					       \
}

void HandleTifFigure  (texutil TeXUtil,char *SuppliedFileName) {
  int TifLittleEndian;
  FILE *TIF;
  unsigned int TifNextChunk;
  char *FileName;
  char *FileSuffix;
  char *TifFile;
  double TifWidth  = 0.0 ;
  double TifHeight = 0.0 ;
  char *TifTitle = NULL ;
  char *TifAuthor = NULL ;
  char *TifCreator = NULL ;
  int TifUnit = 0 ;
  int TifHRes = 1 ;
  int TifVRes = 1 ;
  unsigned long TifSize = 0;
  double TifMult;
  FileName = safe_strdup(SuppliedFileName);
  char *filenamepointer = FileName;
  FileSuffix = SplitFileName (&FileName) ;
  if (FileSuffix==NULL || strcasecmp(FileSuffix,"tif")!=0) {
    safe_free(filenamepointer);
    return;
  }
  safe_free(filenamepointer);
  TifFile = SuppliedFileName ;
  safe_fopen(TIF,TifFile,"rb");
  if (!TIF)
    return;
  fprintf(STDOUT,"%s %s\n", MS[ID_TifFile], SuppliedFileName) ;
  if(!fseek(TIF,0L,SEEK_END)) {
    TifSize = ftell(TIF);
    rewind(TIF);
  } else {
    safe_fclose (TIF) ;
    return;
  }
  char TifByteOrder[3];
  TifByteOrder[2] = 0;
  if(fread(TifByteOrder, 1, 2 , TIF)==2) {
    TifLittleEndian = (STREQ(TifByteOrder,"II"));
  } else {
    safe_fclose (TIF) ;
    return;
  }
  if (TifGetShort(TIF,TifLittleEndian) != 42) {
    safe_fclose ( TIF ) ;
    return ;
  }
  TifNextChunk = TifGetLong(TIF,TifLittleEndian) ;
  while (1) {
    TifGetChunk();
  }
  if (TifUnit==2) {
    TifMult = INtoCM ;
  } else if (TifUnit==3) {
    TifMult = 1.0;
  } else {
    TifMult = 72.0 ;
  }
  TifWidth  = (TifWidth /TifHRes)*TifMult ;
  TifHeight = (TifHeight/TifVRes)*TifMult ;
  safe_fclose ( TIF ) ;
  SaveFigurePresets(TeXUtil, TifFile, "tif", TifUnit,0, 0, TifWidth, TifHeight,
		    (TifTitle?TifTitle:""), (TifCreator?TifCreator:""), TifSize);
  safe_free(TifTitle);
  safe_free(TifAuthor);
  safe_free(TifCreator);
}


#define PngGetByte() getc(PNG)

#define PngGetLong() (getc(PNG)*(256*256*256)+getc(PNG)*(256*256)+getc(PNG)*256+getc(PNG))

#define PngGetChunk(PngSize,PNG) {                           \
  char PngType[5];                                           \
  PngType[4] = 0;					     \
  char PngKeyword[81];                                       \
  PngKeyword[80] = 0;					     \
  unsigned int PngLength;				     \
  if (PngNextChunk<PngSize) { 				     \
    fseek (PNG, PngNextChunk, 0 );			     \
    PngLength = PngGetLong();				     \
    PngNextChunk = PngNextChunk + PngLength + 12 ;	     \
    int res = fread (PngType, 1, 4 , PNG);		     \
    if ((res != 4) || (!strlen(PngType))) {		     \
      break;						     \
    } else if (STREQ(PngType,"IEND")) {			     \
      break;						     \
    } else if (STREQ(PngType,"IHDR")) { 		     \
      PngWidth = (double)PngGetLong() ;			     \
      PngHeight = (double)PngGetLong() ;		     \
    } else if (STREQ(PngType,"pHYs")) {			     \
      PngHRes = PngGetLong() ;				     \
      PngVRes = PngGetLong() ;				     \
      PngUnit = PngGetByte();				     \
    } else if (STREQ(PngType,"tEXt")) { 		     \
      int byte;						     \
      int i=0;						     \
      while((byte = PngGetByte()))			     \
	PngKeyword[i++] = byte;				     \
      PngKeyword[i] = 0;				     \
      if (STREQ(PngKeyword,"Title")) { 			               \
	fread (PngTitle, 1, (PngLength-strlen(PngKeyword)-1), PNG);    \
	PngTitle[PngLength-strlen(PngKeyword)-1] = 0;		       \
      } else if (STREQ(PngKeyword,"Author")) {			       \
	fread (PngAuthor, 1, (PngLength-strlen(PngKeyword)-1), PNG);   \
	PngAuthor[PngLength-strlen(PngKeyword)-1] = 0;		       \
      } else if (STREQ(PngKeyword,"Software")) { 		       \
	fread (PngCreator, 1, (PngLength-strlen(PngKeyword)-1), PNG);  \
	PngCreator[PngLength-strlen(PngKeyword)-1] = 0;                \
      }                                                                \
    }                                                                  \
    continue ;							       \
  } else { 							       \
    break; 							       \
  } 								       \
}

void HandlePngFigure (texutil TeXUtil,char *SuppliedFileName) {
  char *FileName;
  char *FileSuffix;
  char *PngFile;
  char PngSig[9];
  PngSig[8] = 0;
  char *PngSignature = "\211PNG\r\n\032\n";
  int PngNextChunk ;
  double PngWidth = 0.0  ;
  double PngHeight = 0.0  ;
  char *PngTitle = NULL ;
  char *PngAuthor = NULL;
  char *PngCreator = NULL ;
  int PngUnit = 0 ;
  int PngVRes = 1 ;
  int PngHRes = 1 ;
  long int PngSize = 0;
  FileName = safe_strdup(SuppliedFileName);
  char *filenamepointer = FileName;
  FileSuffix = SplitFileName (&FileName) ;
  if (FileSuffix == NULL || strcasecmp(FileSuffix,"png") != 0) {
    safe_free(filenamepointer);
    return;
  }
  safe_free(filenamepointer);
  PngFile = SuppliedFileName ;
  fprintf (STDOUT, "%s %s\n", MS[ID_PngFile], SuppliedFileName );
  FILE *PNG;
  safe_fopen (PNG,PngFile, "rb") ;
  if (!PNG)
    return;
  if(!fseek(PNG,0L,SEEK_END)) {
    PngSize = ftell(PNG);
    rewind(PNG);
  } else {
    safe_fclose (PNG) ;
    return;
  }
  if(fread (PngSig,1,8,PNG) == 8) {
    if (!STREQ(PngSig,PngSignature)) {
      safe_fclose (PNG) ;
      return ;
    }
  } else {
    safe_fclose (PNG) ;
    return ;
  }
  PngNextChunk = 8 ;
  PngTitle = safe_malloc(80);
  *PngTitle = 0;
  PngAuthor = safe_malloc(80);
  *PngAuthor = 0;
  PngCreator = safe_malloc(80);
  *PngCreator = 0;
  while (1) {
    PngGetChunk(PngSize,PNG);
  }
  PngWidth  = (PngWidth /PngVRes) ;
  PngHeight = (PngHeight/PngHRes) ;
  safe_fclose (PNG) ;
  SaveFigurePresets (TeXUtil, PngFile, "png", PngUnit, 0.0, 0.0, PngWidth, PngHeight,
		     PngTitle, PngCreator, PngSize );
  safe_free(PngTitle);
  safe_free(PngAuthor);
  safe_free(PngCreator);
}


#define JpgGetByte()  getc(JPG)

#define JpgGetInteger()  (getc(JPG)*256+getc(JPG))

void
HandleJpgFigure (texutil TeXUtil, char *SuppliedFileName) {
  char *JpgSignature = "JFIF";
  char *FileName;
  char *FileSuffix;
  char JpgSig[6];
  JpgSig[5] = 0;
  char *JpgFile;
  unsigned int JpgVersion = 0;
  unsigned int JpgDummy = 0;
  unsigned int JpgSize = 0;
  double JpgWidth = 0.0 ;
  double JpgHeight = 0.0 ;
  double JpgMult = 1.0 ;
  unsigned int JpgUnit = 0 ;
  unsigned int JpgVRes = 1 ;
  unsigned int JpgHRes = 1 ;
  unsigned int JpgLen = 0 ;
  unsigned int JpgPos = 0;
  unsigned int JpgSoi = 0;
  unsigned int JpgApp = 0;
  FileName = safe_strdup(SuppliedFileName);
  char *filenamepointer = FileName;
  FileSuffix = SplitFileName (&FileName) ;
  if (FileSuffix == NULL || strcasecmp(FileSuffix,"jpg") != 0) {
    safe_free(filenamepointer);
    return;
  }
  safe_free(filenamepointer);
  JpgFile = SuppliedFileName ;
  fprintf (STDOUT, "%s %s\n", MS[ID_JpgFile], SuppliedFileName );
  FILE *JPG;
  safe_fopen (JPG,JpgFile, "rb") ;
  if (!JPG)
    return;
  if(!fseek(JPG,0L,SEEK_END)) {
    JpgSize = ftell(JPG);
    rewind(JPG);
  } else {
    safe_fclose (JPG) ;
    return;
  }
  if(fread (JpgSig, 1, 4, JPG) == 4) {
    if (!(JpgSig[0] == (char)255 &&
	  JpgSig[1] == (char)216 &&
	  JpgSig[2] == (char)255 &&
	  JpgSig[3] == (char)224)) {
      safe_fclose (JPG ) ;
      return ;
    }
  } else {
      safe_fclose (JPG ) ;
      return ;
  }
  JpgLen = JpgGetInteger();
  if(fread (JpgSig, 1, 5, JPG) == 5) {
    if (!STREQ(JpgSig,JpgSignature)) {
      safe_fclose ( JPG ) ;
      return;
    }
  } else {
    safe_fclose ( JPG ) ;
    return;
  }
  JpgUnit = JpgGetByte();
  JpgVersion = JpgGetInteger();
  JpgHRes = JpgGetInteger();
  JpgVRes = JpgGetInteger();
  JpgPos = JpgLen + 4 ;
  JpgSoi = 255 ;
  while (1) {
    fseek (JPG, JpgPos, 0 );
    JpgSoi = JpgGetByte() ;
    JpgApp = JpgGetByte() ;
    JpgLen = JpgGetInteger() ;
    if (JpgSoi!=255)
      goto END ;
    if ((JpgApp>=192) && (JpgApp<=195))  {
      JpgDummy = JpgGetByte();
      JpgHeight = JpgGetInteger();
      JpgWidth = JpgGetInteger();
    }
    JpgPos = JpgPos + JpgLen + 2 ;
  }
 END:
  safe_fclose ( JPG ) ;
  if (JpgUnit==1)
    JpgMult = INtoCM;
  else
    JpgMult = 1.0;
  if (JpgHRes<2)
    JpgHRes = 72;
  if (JpgVRes<2)
    JpgVRes = 72;
  JpgWidth = (JpgWidth/JpgHRes)*JpgMult ;
  JpgHeight = (JpgHeight/JpgVRes)*JpgMult ;
  SaveFigurePresets (TeXUtil, JpgFile, "jpg", JpgUnit, 0, 0, JpgWidth, JpgHeight, "", "", JpgSize );
 }


void FlushFigures (texutil TeXUtil) {
  FILE *TUF;
  int n;
  OutputFile = SetOutputFile (TeXUtil, "texutil.tuf") ;
  safe_fopen (TUF,OutputFile,"w") ;
  if (!TUF)
    exit(1);
  fprintf (TUF, "%%\n%% %s / Figures\n%%\n", Program) ;
  fprintf (TUF, "\\thisisfigureversion{1996.06.01}\n%%\n" );
  for (n=0 ; n<NOfFigures ; ++n) {
    fprintf (TUF, "%s",Figures[n]);
    safe_free(Figures[n]);
  }
  safe_free(Figures);
  safe_fclose (TUF) ;
  if (NOfFigures)
    fprintf(STDOUT,"%s %s\n",MS[ID_OutputFile], OutputFile );
  else
    unlink (OutputFile);
  fprintf(STDOUT,"%s %d\n",MS[ID_NOfFigures], NOfFigures );
}


void DoHandleFigures (texutil TeXUtil, char *FigureSuffix, void(*FigureMethod)(texutil, char *)) {
  string *UserSuppliedFiles = NULL;
  if (!strlen(InputFile))
    InputFile = FigureSuffix ;
  int NOfSuppliedFiles = CheckInputFiles (InputFile, &UserSuppliedFiles) ;
  int k=0;
  while (UserSuppliedFiles[k] != NULL) {
    k++;
  }
  for (k=0; k<NOfSuppliedFiles; k++) {
    (*FigureMethod) (TeXUtil, UserSuppliedFiles[k]);
    safe_free (UserSuppliedFiles[k]);
  }
  safe_free(UserSuppliedFiles);
}

void HandleFigures (texutil TeXUtil, int ARGC,string *ARGV)  {
  int i;
  fprintf(STDOUT,"%s %s\n", MS[ID_Action],  MS[ID_GeneratingFigures]) ;
  for (i=0; i<ARGC; i++) {
    if (strstr(ARGV[i],".eps") != NULL) {
      fprintf(STDOUT,"%s %s\n", MS[ID_Option], MS[ID_UsingEps]) ;
      if (ProcessEpsToPdf)
	fprintf(STDOUT,"%s %s\n", MS[ID_Option], MS[ID_EpsToPdf]);
      if (ProcessEpsPage)
	fprintf(STDOUT,"%s %s\n", MS[ID_Option], MS[ID_EpsPage]);
      i = ARGC;
    }
  }
  for (i=0; i<ARGC; i++) {
    if (strstr(ARGV[i],".pdf") != NULL) {
      fprintf(STDOUT,"%s %s\n", MS[ID_Option], MS[ID_UsingPdf]) ;
      i = ARGC;
    }
  }
  for (i=0; i<ARGC; i++) {
    if (strstr(ARGV[i],".tif") != NULL) {
      fprintf(STDOUT,"%s %s\n", MS[ID_Option], MS[ID_UsingTif]) ;
      i = ARGC;
    }
  }
  for (i=0; i<ARGC; i++) {
    if (strstr(ARGV[i],".png") != NULL) {
      fprintf(STDOUT,"%s %s\n", MS[ID_Option], MS[ID_UsingPng]) ;
      i = ARGC;
    }
  }
  for (i=0; i<ARGC; i++) {
    if (strstr(ARGV[i],".jpg") != NULL) {
      fprintf(STDOUT,"%s %s\n", MS[ID_Option], MS[ID_UsingJpg]) ;
      i = ARGC;
    }
  }
  //InitializeFigures(TeXUtil) ;
  Figures = safe_malloc(sizeof (char *)*Figures_size);
  DoHandleFigures (TeXUtil, "eps", HandleEpsFigure) ;
  DoHandleFigures (TeXUtil, "pdf", HandlePdfFigure) ;
  DoHandleFigures (TeXUtil, "tif", HandleTifFigure) ;
  DoHandleFigures (TeXUtil, "png", HandlePngFigure) ;
  DoHandleFigures (TeXUtil, "jpg", HandleJpgFigure) ;
  FlushFigures(TeXUtil);
}


void InitializeReport (texutil TeXUtil) {
  if (strcmp(UserInterface,"nl") == 0) {
    MS[ID_ProcessingReferences]    = "commando's, lijsten en indexen verwerken" ;
    MS[ID_MergingReferences]       = "indexen samenvoegen" ;
    MS[ID_GeneratingDocumentation] = "ConTeXt documentatie file voorbereiden" ;
    MS[ID_GeneratingSources]       = "ConTeXt broncode file genereren" ;
    MS[ID_FilteringDefinitions]    = "ConTeXt definities filteren" ;
    MS[ID_CopyingTemplates]        = "TeXEdit toets templates copieren" ;
    MS[ID_CopyingInformation]      = "TeXEdit help informatie copieren" ;
    MS[ID_GeneratingFigures]       = "figuur file genereren" ;
    MS[ID_FilteringLogFile]        = "log file filteren (poor mans version)" ;

    MS[ID_SortingIJ]               = "IJ sorteren onder Y" ;
    MS[ID_ConvertingHigh]          = "hoge ASCII waarden converteren" ;
    MS[ID_ProcessingQuotes]        = "characters met accenten afhandelen" ;
    MS[ID_ForcingFileType]         = "filetype instellen" ;
    MS[ID_UsingEps]                = "EPS files afhandelen" ;
    MS[ID_UsingTif]                = "TIF files afhandelen" ;
    MS[ID_UsingPdf]                = "PDF files afhandelen" ;
    MS[ID_UsingPng]                = "PNG files afhandelen" ;
    MS[ID_UsingJpg]                = "JPG files afhandelen" ;
    MS[ID_EpsToPdf]                = "EPS converteren naar PDF";
    MS[ID_EpsPage]                 = "EPS pagina instellen";
    MS[ID_FilteringBoxes]          = "overfull boxes filteren" ;
    MS[ID_ApplyingCriterium]       = "criterium toepassen" ;
    MS[ID_FilteringUnknown]        = "onbekende ... filteren" ;

    MS[ID_NoInputFile]             = "geen invoer file opgegeven" ;
    MS[ID_NoOutputFile]            = "geen uitvoer file gegenereerd" ;
    MS[ID_EmptyInputFile]          = "lege invoer file" ;
    MS[ID_NotYetImplemented]       = "nog niet beschikbaar" ;

    MS[ID_Action]                  = "                 actie :" ;
    MS[ID_Option]                  = "                 optie :" ;
    MS[ID_Error]                   = "                  fout :" ;
    MS[ID_Remark]                  = "             opmerking :" ;
    MS[ID_SystemCall]              = "        systeemaanroep :" ;
    MS[ID_BadSystemCall]           = "  foute systeemaanroep :" ;
    MS[ID_MissingSubroutine]       = "  onbekende subroutine :" ;

    MS[ID_EmbeddedFiles]           = "       gebruikte files :" ;
    MS[ID_BeginEndError]           = "           b/e fout in :" ;
    MS[ID_SynonymEntries]          = "     aantal synoniemen :" ;
    MS[ID_SynonymErrors]           = "                fouten :" ;
    MS[ID_RegisterEntries]         = "       aantal ingangen :" ;
    MS[ID_RegisterErrors]          = "                fouten :" ;
    MS[ID_PassedCommands]          = "     aantal commando's :" ;

    MS[ID_MultiPagePdfFile]        = "      te veel pagina's :" ;
    MS[ID_MissingMediaBox]         = "         geen mediabox :" ;
    MS[ID_MissingBoundingBox]      = "      geen boundingbox :" ;

    MS[ID_NOfDocuments]            = "  documentatie blokken :" ;
    MS[ID_NOfDefinitions]          = "     definitie blokken :" ;
    MS[ID_NOfSkips]                = "  overgeslagen blokken :" ;
    MS[ID_NOfSetups]               = "    gecopieerde setups :" ;
    MS[ID_NOfTemplates]            = " gecopieerde templates :" ;
    MS[ID_NOfInfos]                = " gecopieerde helpinfos :" ;
    MS[ID_NOfFigures]              = "     verwerkte figuren :" ;
    MS[ID_NOfBoxes]                = "        te volle boxen :" ;
    MS[ID_NOfUnknown]              = "         onbekende ... :" ;

    MS[ID_InputFile]               = "           invoer file :" ;
    MS[ID_OutputFile]              = "          outvoer file :" ;
    MS[ID_FileType]                = "             type file :" ;
    MS[ID_EpsFile]                 = "              eps file :" ;
    MS[ID_PdfFile]                 = "              pdf file :" ;
    MS[ID_TifFile]                 = "              tif file :" ;
    MS[ID_PngFile]                 = "              png file :" ;
    MS[ID_JpgFile]                 = "              jpg file :" ;
    MS[ID_MPFile]                  = "         metapost file :" ;

    MS[ID_LoadedFilter]            = "        geladen filter :" ;
    MS[ID_RemappedKeys]            = "     onderschepte keys :" ;
    MS[ID_WrongFilterPath]         = "       fout filter pad :" ;

    MS[ID_Overfull]                = "te vol" ;
    MS[ID_Entries]                 = "ingangen" ;
    MS[ID_References]              = "verwijzingen" ;

    MS[ID_ExtraProgram]            = "       extra programma :" ; /* TH: was missing */
    MS[ID_PlugInInit]              = "    plugin initialized :" ;
    MS[ID_PlugInReport]            = "         plugin report :" ;

    MS[ID_HelpInfo] =
      "          --references   hulp file verwerken / tui->tuo                \n"
      "                       --ij : IJ als Y sorteren                        \n"
      "                       --high : hoge ASCII waarden converteren         \n"
      "                       --quotes : quotes converteren                   \n"
      "                       --tcxpath : tcx filter pad                      \n"
      "                                                                       \n"
      "           --purge(all)    tijdelijke (klad) files verwijderen         \n"
      "                                                                       \n"
      "           --documents   documentatie file genereren / tex->ted        \n"
      "             --sources   broncode file genereren / tex->tes            \n"
      "              --setups   ConTeXt definities filteren / tex->texutil.tus\n"
      "           --templates   TeXEdit templates filteren / tex->tud         \n"
      "               --infos   TeXEdit helpinfo filteren / tex->tud          \n"
      "                                                                       \n"
      "             --figures   eps figuren lijst genereren / *->texutil.tuf  \n"
      "                       --epspage : voorbereiden voor pdf               \n"
      "                       --epstopdf : omzetten naar pdf                  \n"
      "                                                                       \n"
      "             --logfile   logfile filteren / log->%s           \n"
      "                       --box : overfull boxes controleren              \n"
      "                       --criterium : overfull criterium in pt          \n"
      "                       --unknown :onbekende ... controleren            \n";

  } else if (strcmp(UserInterface,"de") == 0) {

    MS[ID_ProcessingReferences]    = "Verarbeiten der Befehle, Listen und Register" ;
    MS[ID_MergingReferences]       = "Register verschmelzen" ;
    MS[ID_GeneratingDocumentation] = "Vorbereiten der ConTeXt-Dokumentationsdatei" ;
    MS[ID_GeneratingSources]       = "Erstellen einer nur Quelltext ConTeXt-Datei" ;
    MS[ID_FilteringDefinitions]    = "Filtern der ConTeXt-Definitionen" ;
    MS[ID_CopyingTemplates]        = "Kopieren der TeXEdit-Test-key-templates" ;
    MS[ID_CopyingInformation]      = "Kopieren der TeXEdit-Hilfsinformation" ;
    MS[ID_GeneratingFigures]       = "Erstellen einer Abb-Uebersichtsdatei" ;
    MS[ID_FilteringLogFile]        = "Filtern der log-Datei" ;

    MS[ID_SortingIJ]               = "Sortiere IJ nach Y" ;
    MS[ID_ConvertingHigh]          = "Konvertiere hohe ASCII-Werte" ;
    MS[ID_ProcessingQuotes]        = "Verarbeiten der Akzentzeichen" ;
    MS[ID_ForcingFileType]         = "Dateityp einstellen" ;
    MS[ID_UsingEps]                = "EPS-Dateien verarbeite" ;
    MS[ID_UsingTif]                = "TIF-Dateien verarbeite" ;
    MS[ID_UsingPdf]                = "PDF-Dateien verarbeite" ;
    MS[ID_UsingPng]                = "PNG-Dateien verarbeite" ;
    MS[ID_UsingJpg]                = "JPG-Dateien verarbeite" ;
    MS[ID_EpsToPdf]                = "convert EPS to PDF";
    MS[ID_EpsPage]                 = "setup EPS page";

    MS[ID_FilteringBoxes]          = "Filtern der ueberfuellten Boxen" ;
    MS[ID_ApplyingCriterium]       = "Anwenden des uebervoll-Kriteriums" ;
    MS[ID_FilteringUnknown]        = "Filter unbekannt ..." ;

    MS[ID_NoInputFile]             = "Keine Eingabedatei angegeben" ;
    MS[ID_NoOutputFile]            = "Keine Ausgabedatei generiert" ;
    MS[ID_EmptyInputFile]          = "Leere Eingabedatei" ;
    MS[ID_NotYetImplemented]       = "Noch nicht verfuegbar" ;

    MS[ID_Action]                  = "                Aktion :" ;
    MS[ID_Option]                  = "                Option :" ;
    MS[ID_Error]                   = "                Fehler :" ;
    MS[ID_Remark]                  = "             Anmerkung :" ;
    MS[ID_SystemCall]              = "           system call :" ;
    MS[ID_BadSystemCall]           = "       bad system call :" ;
    MS[ID_MissingSubroutine]       = "    missing subroutine :" ;
    MS[ID_SystemCall]              = "          Systemaufruf :" ;
    MS[ID_BadSystemCall]           = "   Fehlerhafter Aufruf :" ;
    MS[ID_MissingSubroutine]       = " Fehlende Unterroutine :" ;

    MS[ID_EmbeddedFiles]           = "  Eingebettete Dateien :" ;
    MS[ID_BeginEndError]           = "   Beg./Ende-Fehler in :" ;
    MS[ID_SynonymEntries]          = "      Synonymeintraege :" ;
    MS[ID_SynonymErrors]           = " Fehlerhafte Eintraege :" ;
    MS[ID_RegisterEntries]         = "     Registereintraege :" ;
    MS[ID_RegisterErrors]          = " Fehlerhafte Eintraege :" ;
    MS[ID_PassedCommands]          = "    Verarbeite Befehle :" ;

    MS[ID_MultiPagePdfFile]        = "       zu viele Seiten :" ;
    MS[ID_MissingMediaBox]         = "     fehlende mediabox :" ;
    MS[ID_MissingBoundingBox]      = "  fehlende boundingbox :" ;

    MS[ID_NOfDocuments]            = "       Dokumentbloecke :" ;
    MS[ID_NOfDefinitions]          = "    Definitionsbloecke :" ;
    MS[ID_NOfSkips]                = "Uebersprungene Bloecke :" ;
    MS[ID_NOfSetups]               = "       Kopierte setups :" ;
    MS[ID_NOfTemplates]            = "    Kopierte templates :" ;
    MS[ID_NOfInfos]                = "    Kopierte helpinfos :" ;
    MS[ID_NOfFigures]              = "     Verarbeitete Abb. :" ;
    MS[ID_NOfBoxes]                = "        Zu volle Boxen :" ;
    MS[ID_NOfUnknown]              = "         Unbekannt ... :" ;

    MS[ID_InputFile]               = "          Eingabedatei :" ;
    MS[ID_OutputFile]              = "          Ausgabedatei :" ;
    MS[ID_FileType]                = "              Dateityp :" ;
    MS[ID_EpsFile]                 = "             eps-Datei :" ;
    MS[ID_PdfFile]                 = "             pdf-Datei :" ;
    MS[ID_TifFile]                 = "             tif-Datei :" ;
    MS[ID_PngFile]                 = "             png-Datei :" ;
    MS[ID_JpgFile]                 = "             jpg-Datei :" ;
    MS[ID_MPFile]                  = "        metapost-Datei :" ;

    MS[ID_LoadedFilter]            = "         loaded filter :" ;
    MS[ID_RemappedKeys]            = "         remapped keys :" ;
    MS[ID_WrongFilterPath]         = "     wrong filter path :" ;

    MS[ID_Overfull]                = "zu voll" ;
    MS[ID_Entries]                 = "Eintraege" ;
    MS[ID_References]              = "Referenzen" ;

    MS[ID_ExtraProgram]            = "         extra program :" ;
    MS[ID_PlugInInit]              = "    plugin initialized :" ;
    MS[ID_PlugInReport]            = "         plugin report :" ;

    MS[ID_HelpInfo] =
      "          --references   Verarbeiten der Hilfsdatei / tui->tuo          \n"
      "                       --ij : Sortiere IJ als Y                         \n"
      "                       --high : Konvertiere hohe ASCII-Werte            \n"
      "                       --quotes : Konvertiere akzentuierte Buchstaben   \n"
      "                       --tcxpath : tcx Filter Path                      \n"
      "                                                                        \n"
      "          --purge(all)    entferne temporaere ConTeXt-Dateien           \n"
      "                                                                        \n"
      "           --documents   Erstelle Dokumentationsdatei / tex->ted        \n"
      "             --sources   Erstelle reine Quelltextdateien / tex->tes     \n"
      "              --setups   Filtere ConTeXt-Definitionen / tex->texutil.tus\n"
      "           --templates   Filtere TeXEdit-templates / tex->tud           \n"
      "               --infos   Filtere TeXEdit-helpinfo / tex->tud            \n"
      "                                                                        \n"
      "             --figures   Erstelle eps-Abbildungsliste / *->texutil.tuf  \n"
      "                       --epspage : Bereite fuer pdf vor                 \n"
      "                       --epstopdf : Konvertiere zu pdf                  \n"
      "                                                                        \n"
      "             --logfile   Filtere log-Datei / log->%s           \n"
      "                       --box : Ueberpruefe uebervolle Boxen             \n"
      "                       --criterium : Uebervoll-Kriterium in pt          \n"
      "                       --unknown : Ueberpruefe auf unbekannte ...       \n";

  } else if (strcmp(UserInterface,"it") == 0) {

    MS[ID_ProcessingReferences]    = "elaborazione di comandi, liste e registri" ;
    MS[ID_MergingReferences]       = "fusione dei registri" ;
    MS[ID_GeneratingDocumentation] = "preparazione del file di documentazione ConTeXt" ;
    MS[ID_GeneratingSources]       = "generazione del solo sorgente ConTeXt" ;
    MS[ID_FilteringDefinitions]    = "filtraggio delle definizioni formali ConTeXt" ;
    MS[ID_CopyingTemplates]        = "copia dei modelli rapidi di voci di TeXEdit" ;
    MS[ID_CopyingInformation]      = "copia delle informazioni di aiuto di TeXEdit" ;
    MS[ID_GeneratingFigures]       = "generazione del file di elengo delle figure" ;
    MS[ID_FilteringLogFile]        = "filtraggio del file di log" ;

    MS[ID_SortingIJ]               = "IJ elencato sotto Y" ;
    MS[ID_ConvertingHigh]          = "conversione dei valori ASCII alti" ;
    MS[ID_ProcessingQuotes]        = "elaborazione dei caratteri accentati" ;
    MS[ID_ForcingFileType]         = "impostazine del tipo di file" ;
    MS[ID_UsingEps]                = "elaborazione del file EPS" ;
    MS[ID_UsingTif]                = "elaborazione del file TIF" ;
    MS[ID_UsingPdf]                = "elaborazione del file PDF" ;
    MS[ID_UsingPng]                = "elaborazione del file PNG" ;
    MS[ID_UsingJpg]                = "elaborazione del file JPG" ;
    MS[ID_EpsToPdf]                = "conversione da EPS a PDF";
    MS[ID_EpsPage]                 = "impostazione pagina EPS";

    MS[ID_FilteringBoxes]          = "filtraggio delle overfull boxes" ;
    MS[ID_ApplyingCriterium]       = "applicazione del criterio overfull" ;
    MS[ID_FilteringUnknown]        = "filtraggio dei messaggi non conosciuti ..." ;

    MS[ID_NoInputFile]             = "nessun file di input specificato" ;
    MS[ID_NoOutputFile]            = "nessun file di output generato" ;
    MS[ID_EmptyInputFile]          = "file di input vuoto" ;
    MS[ID_NotYetImplemented]       = "non ancora disponibile" ;

    MS[ID_Action]                  = "                azione :" ;
    MS[ID_Option]                  = "               opzione :" ;
    MS[ID_Error]                   = "                errore :" ;
    MS[ID_Remark]                  = "              commento :" ;
    MS[ID_SystemCall]              = "   chiamata di sistema :" ;
    MS[ID_BadSystemCall]           = "chiamata di sistema er :" ;
    MS[ID_MissingSubroutine]       = "   subroutine mancante :" ;

    MS[ID_EmbeddedFiles]           = "          file inclusi :" ;
    MS[ID_BeginEndError]           = "      errore di i/f in :" ;
    MS[ID_SynonymEntries]          = "      voci di sinonimi :" ;
    MS[ID_SynonymErrors]           = "           voci errate :" ;
    MS[ID_RegisterEntries]         = "      voci di registro :" ;
    MS[ID_RegisterErrors]          = "           voci errate :" ;
    MS[ID_PassedCommands]          = "       comandi passati :" ;

    MS[ID_MultiPagePdfFile]        = "         troppe pagine :" ;
    MS[ID_MissingMediaBox]         = "     mediabox mancante :" ;
    MS[ID_MissingBoundingBox]      = "  boundingbox mancante :" ;

    MS[ID_NOfDocuments]            = "  blocchi di documento :" ;
    MS[ID_NOfDefinitions]          = "blocchi di definizioni :" ;
    MS[ID_NOfSkips]                = "       blocchi saltati :" ;
    MS[ID_NOfSetups]               = "  impostazioni copiate :" ;
    MS[ID_NOfTemplates]            = "       modelli copiati :" ;
    MS[ID_NOfInfos]                = "      helpinfo copiati :" ;
    MS[ID_NOfFigures]              = "      figure elaborate :" ;
    MS[ID_NOfBoxes]                = "        overfull boxes :" ;
    MS[ID_NOfUnknown]              = "       sconosciuti ... :" ;

    MS[ID_InputFile]               = "         file di input :" ;
    MS[ID_OutputFile]              = "        file di output :" ;
    MS[ID_FileType]                = "          tipo di file :" ;
    MS[ID_EpsFile]                 = "              file eps :" ;
    MS[ID_PdfFile]                 = "              file pdf :" ;
    MS[ID_TifFile]                 = "              file tif :" ;
    MS[ID_PngFile]                 = "              file png :" ;
    MS[ID_JpgFile]                 = "              file jpg :" ;
    MS[ID_MPFile]                  = "         file metapost :" ;

    MS[ID_LoadedFilter]            = "       filtro caricato :" ;
    MS[ID_RemappedKeys]            = "        voci rimappate :" ;
    MS[ID_WrongFilterPath]         = "percorso filtro errato :" ;

    MS[ID_Overfull]                = "overfull" ;
    MS[ID_Entries]                 = "voci" ;
    MS[ID_References]              = "riferimenti" ;

    MS[ID_ExtraProgram]            = "         extra program :" ;
    MS[ID_PlugInInit]              = "    plugin initialized :" ;
    MS[ID_PlugInReport]            = "         plugin report :" ;

    MS[ID_HelpInfo] =
       "          --references   elabora file ausiliari / tui->tuo              \n"
       "                       --ij : elenca IJ come Y                          \n"
       "                       --high : converti i valori ASCII alti            \n"
       "                       --quotes : converti caratteri accentati          \n"
       "                       --tcxpath : percorso del filtro tcx              \n"
       "                                                                        \n"
       "         --purge(all)    rimuovi i file temporanei ConTeXt              \n"
       "                                                                        \n"
       "           --documents   genera file di documentazione / tex->ted       \n"
       "             --sources   genera solo sorgente / tex->tes                \n"
       "              --setups   filtra definizioni ConTeXt / tex->texutil.tus  \n"
       "           --templates   filtra modelli TeXEdit / tex->tud              \n"
       "               --infos   filtra helpinfo TeXEdit / tex->tud             \n"
       "                                                                        \n"
       "             --figures   genera lista figure eps / *->texutil.tuf       \n"
       "                       --epspage : prepara per pdf                      \n"
       "                       --epstopdf : converti in pdf                     \n"
       "                                                                        \n"
       "             --logfile   filtra logfile / log->%s              \n"
       "                       --box : controlla overful boxes                  \n"
       "                       --criterium : criterio overfull in pt            \n"
       "                       --unknown : controlla sconosciuti ...            \n";

  } else {

    MS[ID_ProcessingReferences]    = "processing commands, lists and registers" ;
    MS[ID_MergingReferences]       = "merging registers" ;
    MS[ID_GeneratingDocumentation] = "preparing ConTeXt documentation file" ;
    MS[ID_GeneratingSources]       = "generating ConTeXt source only file" ;
    MS[ID_FilteringDefinitions]    = "filtering formal ConTeXt definitions" ;
    MS[ID_CopyingTemplates]        = "copying TeXEdit quick key templates" ;
    MS[ID_CopyingInformation]      = "copying TeXEdit help information" ;
    MS[ID_GeneratingFigures]       = "generating figure directory file" ;
    MS[ID_FilteringLogFile]        = "filtering log file" ;

    MS[ID_SortingIJ]               = "sorting IJ under Y" ;
    MS[ID_ConvertingHigh]          = "converting high ASCII values" ;
    MS[ID_ProcessingQuotes]        = "handling accented characters" ;
    MS[ID_ForcingFileType]         = "setting up filetype" ;
    MS[ID_UsingEps]                = "processing EPS-file" ;
    MS[ID_UsingTif]                = "processing TIF-file" ;
    MS[ID_UsingPdf]                = "processing PDF-file" ;
    MS[ID_UsingPng]                = "processing PNG-file" ;
    MS[ID_UsingJpg]                = "processing JPG-file" ;
    MS[ID_EpsToPdf]                = "convert EPS to PDF";
    MS[ID_EpsPage]                 = "setup EPS page";

    MS[ID_FilteringBoxes]          = "filtering overfull boxes" ;
    MS[ID_ApplyingCriterium]       = "applying overfull criterium" ;
    MS[ID_FilteringUnknown]        = "filtering unknown ..." ;

    MS[ID_NoInputFile]             = "no input file given" ;
    MS[ID_NoOutputFile]            = "no output file generated" ;
    MS[ID_EmptyInputFile]          = "empty input file" ;
    MS[ID_NotYetImplemented]       = "not yet available" ;

    MS[ID_Action]                  = "                action :" ;
    MS[ID_Option]                  = "                option :" ;
    MS[ID_Error]                   = "                 error :" ;
    MS[ID_Remark]                  = "                remark :" ;
    MS[ID_SystemCall]              = "           system call :" ;
    MS[ID_BadSystemCall]           = "       bad system call :" ;
    MS[ID_MissingSubroutine]       = "    missing subroutine :" ;

    MS[ID_EmbeddedFiles]           = "        embedded files :" ;
    MS[ID_BeginEndError]           = "          b/e error in :" ;
    MS[ID_SynonymEntries]          = "       synonym entries :" ;
    MS[ID_SynonymErrors]           = "           bad entries :" ;
    MS[ID_RegisterEntries]         = "      register entries :" ;
    MS[ID_RegisterErrors]          = "           bad entries :" ;
    MS[ID_PassedCommands]          = "       passed commands :" ;

    MS[ID_MultiPagePdfFile]        = "        too many pages :" ;
    MS[ID_MissingMediaBox]         = "      missing mediabox :" ;
    MS[ID_MissingBoundingBox]      = "   missing boundingbox :" ;

    MS[ID_NOfDocuments]            = "       document blocks :" ;
    MS[ID_NOfDefinitions]          = "     definition blocks :" ;
    MS[ID_NOfSkips]                = "        skipped blocks :" ;
    MS[ID_NOfSetups]               = "         copied setups :" ;
    MS[ID_NOfTemplates]            = "      copied templates :" ;
    MS[ID_NOfInfos]                = "      copied helpinfos :" ;
    MS[ID_NOfFigures]              = "     processed figures :" ;
    MS[ID_NOfBoxes]                = "        overfull boxes :" ;
    MS[ID_NOfUnknown]              = "           unknown ... :" ;

    MS[ID_InputFile]               = "            input file :" ;
    MS[ID_OutputFile]              = "           output file :" ;
    MS[ID_FileType]                = "             file type :" ;
    MS[ID_EpsFile]                 = "              eps file :" ;
    MS[ID_PdfFile]                 = "              pdf file :" ;
    MS[ID_TifFile]                 = "              tif file :" ;
    MS[ID_PngFile]                 = "              png file :" ;
    MS[ID_JpgFile]                 = "              jpg file :" ;
    MS[ID_MPFile]                  = "         metapost file :" ;

    MS[ID_LoadedFilter]            = "         loaded filter :" ;
    MS[ID_RemappedKeys]            = "         remapped keys :" ;
    MS[ID_WrongFilterPath]         = "     wrong filter path :" ;

    MS[ID_Overfull]                = "overfull" ;
    MS[ID_Entries]                 = "entries" ;
    MS[ID_References]              = "references" ;

    MS[ID_ExtraProgram]            = "         extra program :" ;
    MS[ID_PlugInInit]              = "    plugin initialized :" ;
    MS[ID_PlugInReport]            = "         plugin report :" ;

    MS[ID_HelpInfo] =
      "          --references   process auxiliary file / tui->tuo              \n"
      "                       --ij : sort IJ as Y                              \n"
      "                       --high : convert high ASCII values               \n"
      "                       --quotes : convert quotes characters             \n"
      "                       --tcxpath : tcx filter path                      \n"
      "                                                                        \n"
      "         --purge(all)    clean up temporary context files               \n"
      "                                                                        \n"
      "           --documents   generate documentation file / tex->ted         \n"
      "             --sources   generate source only file / tex->tes           \n"
      "              --setups   filter ConTeXt definitions / tex->texutil.tus  \n"
      "           --templates   filter TeXEdit templates / tex->tud            \n"
      "               --infos   filter TeXEdit helpinfo / tex->tud             \n"
      "                                                                        \n"
      "             --figures   generate eps figure list / *->texutil.tuf      \n"
      "                       --epspage : prepare for pdf                      \n"
      "                       --epstopdf : convert to pdf                      \n"
      "                                                                        \n"
      "             --logfile   filter logfile / log->%s              \n"
      "                       --box : check overful boxes                      \n"
      "                       --criterium : overfull criterium in pt           \n"
      "                       --unknown : check unknown ...                    \n";
  }
}

int has_obj (char *line, int *val) {
  /* $old =~ /^(\d+)\s+0\s+obj/o) */
  char *move = line;
  char *test;
  int myval=0;
  if (!isdigit(*move))
    return 0;
  if (strstr(move,"obj") == NULL)
    return 0;
  while (isdigit(*move)) {
    myval = myval*10;
    myval += (*move - '0');
    move++;
  }
  if (isspace(*move)) {
    while (isspace(*move))
      move++;
    if (*move == '0') {
      move++;
      if (isspace(*move)) {
	while (isspace(*move))
	  move++;
	test = strstr(move,"obj");
	if (test == move) {
	  *val = myval;
	  return 1;
	}
      }
    }
  }
  return 0;
}

void DoFilterPages (texutil TeXUtil, char *filename) {
  char *pdfname =  NULL;
  my_concat(&pdfname,filename,".pdf") ;
  if (!file_exists(pdfname)) {
    safe_free(pdfname);
    return;
  }
  char *tuoname =  NULL;
  my_concat(&tuoname,filename,".tuo") ;
  FILE *PDF;
  char *old = NULL;
  int n=0;
  int p=0;
  char *inputline= safe_malloc(BUFSIZE);
  safe_fopen(PDF,pdfname,"rb");
  safe_fopen(TUO,tuoname,"a");
  if (PDF && TUO) {
    safe_free(pdfname);
    safe_free(tuoname);
    while (readline(PDF,&inputline))  {
      inputline = chomp (inputline);
      if ((old!=NULL) && (STREQ(inputline,"/Type /Page")) && (has_obj(old,&p))) {
	++n ;
	fprintf (TUO,"\\objectreference{PDFP}{%d}{%d}{%d}\n",n,p,n);
	/* throw an unneeded line away: */
	readline(PDF,&inputline);
	safe_free (old);
	old = safe_strdup(inputline);
      } else {
	safe_free (old);
	old = safe_strdup(inputline);
      }
    }
    safe_free (old);
  }
  if (PDF)
    safe_fclose(PDF) ;
  if (TUO)
    safe_fclose(TUO) ;
  safe_free(inputline);
}

#define FlushLogTopic() {                           \
  if (!TopicFound) {                                \
    TopicFound = 1 ;                                \
    fprintf (ALL,"\n%% File: %s.log\n\n", FileName);\
  } }

void HandleLogFile (texutil TeXUtil) {
  char *FileName;
  char *FileSuffix;
  int k;
  char *inputline=NULL;
  char *SomeLine;
  char *SomePoints;
  int NOfBoxes ;
  int NOfMatching ;
  int NOfUnknown ;
  int TopicFound ;
  FILE *ALL;
  FILE *LOG;
  if (ProcessBox) {
    fprintf(STDOUT,"%s %s %s\n", MS[ID_Option], MS[ID_FilteringBoxes], "(\\vbox & \\hbox)") ;
    ProcessHBox = 1;
    ProcessVBox = 1;
  } else if (ProcessHBox) {
    fprintf(STDOUT,"%s %s %s\n", MS[ID_Option], MS[ID_FilteringBoxes], "(\\hbox)") ;
    ProcessBox = 1;
  } else if  (ProcessVBox) {
    fprintf(STDOUT,"%s %s %s\n", MS[ID_Option], MS[ID_FilteringBoxes], "(\\vbox)") ;
    ProcessBox = 1;
  }
  if (ProcessBox && ProcessCriterium)
    fprintf(STDOUT,"%s %s\n", MS[ID_Option], MS[ID_ApplyingCriterium]) ;
  if (ProcessUnknown)
    fprintf(STDOUT,"%s %s\n", MS[ID_Option], MS[ID_FilteringUnknown]) ;
  if (!(ProcessBox || ProcessUnknown)) {
    ShowHelpInfo(TeXUtil) ;
    return;
  }
  fprintf(STDOUT,"%s %s\n", MS[ID_Action], MS[ID_FilteringLogFile]) ;
  if (!strlen(InputFile)) {
    fprintf(STDOUT,"%s %s\n", MS[ID_Error], MS[ID_NoInputFile]) ;
  } else {
    NOfBoxes = 0 ;
    NOfMatching = 0 ;
    NOfUnknown = 0 ;
    OutputFile = SetOutputFile (TeXUtil, ProgramLog) ;
    fprintf(STDOUT,"%s %s\n", MS[ID_OutputFile], OutputFile) ;
    string *UserSuppliedFiles;
    int NOfSuppliedFiles = CheckInputFiles (InputFile,&UserSuppliedFiles) ;
    safe_fopen (ALL,OutputFile, "w") ;
    if (NOfSuppliedFiles)
      inputline = safe_malloc(BUFSIZE);
    for (k=0; k<NOfSuppliedFiles; k++) {
      FileName = UserSuppliedFiles[k];
      FileSuffix = SplitFileName (&FileName) ;
      if (FileSuffix == NULL)
	FileSuffix = "";
      char *logfile = NULL;
      my_concat(&logfile,FileName,".log");
      char *texfile = NULL;
      my_concat(&texfile,FileName,".tex");
      safe_fopen (LOG,logfile,"r");
      if (!LOG) {
	fprintf(STDOUT,"%s %s %s.%s\n", MS[ID_Error], MS[ID_EmptyInputFile], FileName,FileSuffix) ;
      } else if (file_exists(texfile)) {
	TopicFound = 0 ;
	fprintf(STDOUT,"%s %s.log\n", MS[ID_InputFile], FileName) ;
	while (readline(LOG,&inputline)) {
	  SomeLine = chomp (inputline);
	  if (ProcessBox &&
	      ((ProcessHBox && (strstr(SomeLine,"Overfull \\hbox")!= NULL)) ||
	       (ProcessVBox && (strstr(SomeLine,"Overfull \\vbox")!= NULL)))) {
	    ++NOfBoxes ;
	    char *SomePointsP = safe_strdup (SomeLine);
	    SomePoints = strstr(SomePointsP,"Overfull ")+strlen("Overfull \\vbox")+2 ; /* " (" */
	    char *temp;
	    temp = strstr(SomePoints,"pt");
	    *temp =0;
	    if (atof(SomePoints)>=ProcessCriterium) {
	      ++NOfMatching ;
	      FlushLogTopic() ;
	      fprintf (ALL, "%s\n", SomeLine );
	      readline(LOG,&inputline);
	      SomeLine=inputline ;
	      fprintf (ALL,"%s", SomeLine);
	    }
	    safe_free(SomePointsP);
	  }

	  if (ProcessUnknown) {
	    char *someline = safe_strdup(SomeLine);
	    someline = lc(someline);
	    if(  (strstr(someline,"onbekende verwijzing") != NULL) ||
		 (strstr(someline,"unbekannte referenz")  != NULL) ||
		 (strstr(someline,"unknown reference")    != NULL) ||
		 (strstr(someline,"dubbele verwijzing")   != NULL) ||
		 (strstr(someline,"duplicate reference")  != NULL) || 
		 (strstr(someline,"doppelte referenz")    != NULL)  ) {
	      ++NOfUnknown ;
	      FlushLogTopic() ;
	      fprintf (ALL,"%s\n", SomeLine);
	    }
	    safe_free(someline);
	  }
	}
      }
      safe_free(texfile);
      safe_free(logfile);
      safe_free(UserSuppliedFiles[k]);
    }
    safe_free(UserSuppliedFiles);
    safe_fclose (ALL) ;
    if (!(NOfBoxes||NOfUnknown))
      unlink (OutputFile);
    if (ProcessBox)
      fprintf(STDOUT,"%s %d -> %d %s\n", MS[ID_NOfBoxes],NOfBoxes,NOfMatching, MS[ID_Overfull]) ;
    if (ProcessUnknown)
      fprintf(STDOUT,"%s %d\n", MS[ID_NOfUnknown], NOfUnknown) ;
  }
}



int
find_words (char *a, char *b,char *line) {
  int retval = 0;
  char *move = line;
 RESTART:
  move = strstr(move,a);
  if (move != NULL) {
    move += strlen(a);
    while (isspace(*move))
      move++;
    move = strstr(move,b);
    if (move != NULL) {
      move += strlen(b);
      retval++;
      goto RESTART;
    }
  }
  return retval;
}

int
find_objects (char *line) {
  int retval = 0;
  char *move = line;
  while (*move && !isdigit(*move))   /* first part wasnt needed */
    *move++;
  char *savedmove = move;
  char *test;
 RESTART:
  if (strstr(move,"obj") == NULL)
    return retval;
  while (isdigit(*move))
    move++;
  if (isspace(*move)) {
    while (isspace(*move))
      move++;
    savedmove = move;
    if (isdigit(*move)) {
      while (isdigit(*move))
	move++;
      if (isspace(*move)) {
	while (isspace(*move))
	  move++;
	test = strstr(move,"obj");
	if (test == move) {
	  retval++;
	  move +=3;
	} else {
	  move = savedmove;
	  goto RESTART;
	}
      } else {
	goto RESTART;
      }
    } else {
      goto RESTART;
    }
  } else {
    goto RESTART;
  }
  return retval;
}

void DoAnalyzeFile (texutil TeXUtil, char *filename) {
  FILE *PDF;
  char *inputline=NULL;
  if (!((strstr(filename,".pdf") != NULL)&&file_exists(filename))) {
    return;
  }
  long int filesize;
  safe_fopen (PDF,filename,"rb") ;
  if(!fseek(PDF,0L,SEEK_END)) {
    filesize = ftell(PDF);
    rewind(PDF);
  } else {
    return;
  }
  fprintf(STDOUT, "        analyzing file : %s\n", filename);
  fprintf(STDOUT, "             file size : %ld\n", filesize);
  int Object = 0 ;
  int Annot = 0 ;
  int Link = 0 ;
  int Widget = 0 ;
  int Named = 0 ;
  int Script = 0 ;
  int Cross = 0 ;
  inputline = safe_malloc(BUFSIZE);
  while (readline(PDF,&inputline)) {
    Object += find_objects(inputline);
    Annot  += find_words("/Type","/Annot"       ,inputline);
    Cross  += find_words("/GoToR","/F"          ,inputline);
    Link   += find_words("/Subtype","/Link"     ,inputline);
    Widget += find_words("/Subtype","/Widget"   ,inputline);
    Named  += find_words("/S","/Named"          ,inputline);
    Script += find_words("/S","/JavaScript"     ,inputline);
  }
  safe_free(inputline);
  safe_fclose (PDF) ;
  fprintf(STDOUT, "               objects : %d\n",Object );
  fprintf(STDOUT, "           annotations : %d\n",Annot);
  fprintf(STDOUT, "                 links : %d (%d named / %d scripts / %d files)\n", Link,Named,Script,Cross );
  fprintf(STDOUT, "               widgets : %d\n", Widget );
}

void GenerateSciteApi (texutil TeXUtil, char *filename) {
  FILE *API;
  int commands = 0 ;
  int environments = 0 ;
  int iNOfCommands = 0;
  string *Collection;
  int Collection_size = 100;
  int Collection_add = 100;
  char *inputline=NULL;
  int ii;
  char *xmlfile = NULL;
  my_concat(&xmlfile,filename,".xml");
  if(!file_exists(xmlfile)) {
    safe_free(xmlfile);
    return;
  }
  fprintf(STDOUT,"        scite api file : %s-scite.api\n",filename );
  fprintf(STDOUT,"      scite lexer file : %s-scite.properties\n", filename);
  FILE *XML;
  safe_fopen(XML,xmlfile,"r");
  safe_free(xmlfile);
  if (XML) {
    inputline = safe_malloc(BUFSIZE);
    Collection = safe_malloc(sizeof(char *)*Collection_size);
    while (readline(XML,&inputline)) {
      inputline = chomp (inputline);
      EXTEND_ARRAY(Collection,iNOfCommands);
      char *temp = strstr(inputline,"<cd:command");
      if (temp != NULL) {
	temp+=strlen("<cd:command");
	while (isspace(*temp))
	  temp++;
	char *tempb = strstr(temp,"name=\"");
	if (tempb != NULL) {
	  tempb+=strlen("name=\"");
	  char *temp = strstr(tempb,"\"");
	  if (temp != NULL) {
	    *temp = 0;
	    char *cmdname = safe_strdup(tempb);
	    temp++;
	    while (isspace(*temp))
	      temp++;
	    tempb = strstr(temp,"type=\"environment\"");
	    if (tempb == NULL) {
	      tempb = strstr(temp,">");
	      if (tempb != NULL) {
		commands++ ;
		Collection[iNOfCommands++] = cmdname;
	      }
	    } else {
	      tempb +=strlen("type=\"environment\"");
	      temp = strstr(tempb,">");
	      if (temp != NULL) {
		environments++ ;
		char *xcommand = NULL;
		my_concat(&xcommand,"start",cmdname);
		Collection[iNOfCommands++] = xcommand;
		EXTEND_ARRAY(Collection,iNOfCommands);
		my_concat(&xcommand,"start",cmdname);
		Collection[iNOfCommands++] = xcommand;
		safe_free(xcommand);
	      }
	    }
	  }
	}
      }
    }
    safe_fclose(XML) ;
    char *apifile = NULL;
    my_concat(&apifile,filename,"-scite.api");
    safe_fopen(API,apifile,"w");
    if (API) {
      for (ii = 0; ii < iNOfCommands; ii++) {
	fprintf (API, "\\%s\n",Collection[ii]);
      }
      fprintf (API, "\n") ;
      safe_fclose(API);
    }
    my_concat(&apifile,filename,"-scite.properties");
    safe_fopen(API,apifile,"w");
    if (API) {
      int i = 0 ;
      char *interface = "en" ;
      char *test= strstr(filename, "cont-");
      if (test != NULL) {
	interface[0] = *(test+5);
	interface[1] = *(test+6);
      }
      fprintf(API, "keywordclass.macros.context.%s=", interface) ;
      for (ii = 0; ii < iNOfCommands; ii++) {
	if (i==0) {
	  fprintf (API, "\\\n    " );
	  i = 5 ;
	} else {
	  i-- ;
	}
	fprintf (API, "%s " , Collection[ii]);
	safe_free(Collection[ii]);
      }
      safe_free(Collection);
      fprintf (API, "\n" );
      safe_fclose(API);
    }
    safe_free(apifile);
  }
  safe_free(inputline);
  fprintf(STDOUT, "              commands : %d\n", commands );
  fprintf(STDOUT, "          environments : %d\n", environments);
}

int texutil_main (texutil TeXUtil, int ARGC,  string *ARGV) {
  discover_init("texutil");
  InputFile = NULL;
  if (ARGC)
    InputFile = strdup(ARGV[0]);
  if (ARGC>1) {
    int i;
    for (i = 1; i < ARGC; i++) {
	  char **d = &InputFile;
	  char *s = InputFile;
	  char *t = " ";
	  char *u = ARGV[i];
	  int rover = 0;
	  char *r = (char *)malloc ((strlen(s)+strlen(t)+strlen(u)+1));
	  if (!r)
		exit (1);
	  while (*s) r[rover++] = *s++;
	  while (*t) r[rover++] = *t++;
	  while (*u) r[rover++] = *u++;
	  r[rover] = 0;
	  free(*d);
	  *d = r;
	  InputFile = *d;
    }
  }
  if(InputFile == NULL)
    InputFile = "";
  if (ProcessSilent)
    RedirectTerminal (TeXUtil);
  else
    ProcessVerbose = 0 ;
  InitializeReport(TeXUtil);
  ShowBanner(TeXUtil);
  if       (UnknownOptions   ) { ShowHelpInfo(TeXUtil);              }
  else if  (ProcessReferences) { HandleReferences(TeXUtil,ARGC,ARGV);}
  else if  (ProcessDocuments ) { HandleDocuments(TeXUtil);           }
  else if  (ProcessSources   ) { HandleSources(TeXUtil);             }
  else if  (ProcessSetups    ) { HandleSetups(TeXUtil);              }
  else if  (ProcessTemplates ) { HandleEditorCues(TeXUtil);          }
  else if  (ProcessInfos     ) { HandleEditorCues(TeXUtil);          }
  else if  (ProcessFigures   ) { HandleFigures(TeXUtil,ARGC,ARGV);   }
  else if  (ProcessLogFile   ) { HandleLogFile(TeXUtil);             }
  else if  (PurgeFiles       ) { DoPurgeFiles(TeXUtil,ARGV);         }
  else if  (PurgeAllFiles    ) { DoPurgeFiles(TeXUtil,ARGV);         }
  else if  (AnalyzeFile      ) { DoAnalyzeFile(TeXUtil,ARGV[0]);     }
  else if  (FilterPages      ) { DoFilterPages(TeXUtil,ARGV[0]);     }
  else if  (SciteApi         ) { GenerateSciteApi(TeXUtil,ARGV[0]);  }
  else if  (ProcessHelp      ) { ShowHelpInfo(TeXUtil);              }
  else                         { ShowHelpInfo(TeXUtil);              }
  return 0;
}



texutil texutil_new (void) {
  static const struct texutilstruct dummy = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
					     "","","",NULL,0.0,"en",NULL,"texutil.log",NULL,NULL,0,
					     NULL,10,10,0,NULL,100,100,NULL,100,100,NULL,NULL,10,10,0,
					     NULL,100,100,0,0,0,0,0,0,0,0,0,0,{NULL}};

  texutil TeXUtil = malloc (sizeof(struct texutilstruct));
  memcpy(TeXUtil,&dummy,sizeof(struct texutilstruct));
  STDOUT  = stdout;
  return TeXUtil;
}

