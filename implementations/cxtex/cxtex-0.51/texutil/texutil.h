/* This is part of the C version of pragma's 'texutil.pl' perl script 
 * 
 * $Id: texutil.h,v 1.1 2004/03/11 10:35:39 taco Exp taco $ 
 */

#ifndef KPATHSEA_TYPES_H
typedef char *string;
#endif

/* The IDs of the message strings */
typedef enum {
ID_ProcessingReferences, ID_MergingReferences,
ID_GeneratingDocumentation, ID_GeneratingSources,
ID_FilteringDefinitions, ID_CopyingTemplates, ID_CopyingInformation,
ID_GeneratingFigures, ID_FilteringLogFile,
ID_SortingIJ, ID_ConvertingHigh, ID_ProcessingQuotes, ID_ForcingFileType,
ID_UsingEps, ID_UsingTif, ID_UsingPdf, ID_UsingPng, ID_UsingJpg,
ID_EpsToPdf, ID_EpsPage,
ID_FilteringBoxes, ID_ApplyingCriterium, ID_FilteringUnknown,
ID_NoInputFile, ID_NoOutputFile, ID_EmptyInputFile, ID_NotYetImplemented,
ID_Action, ID_Option, ID_Error, ID_Remark,
ID_SystemCall, ID_BadSystemCall,
ID_MissingSubroutine, ID_EmbeddedFiles, ID_BeginEndError,
ID_SynonymEntries, ID_SynonymErrors,
ID_RegisterEntries, ID_RegisterErrors, ID_PassedCommands,
ID_MultiPagePdfFile,ID_MissingMediaBox, ID_MissingBoundingBox,
ID_NOfDocuments, ID_NOfDefinitions, ID_NOfSkips, ID_NOfSetups,
ID_NOfTemplates, ID_NOfInfos, ID_NOfFigures, ID_NOfBoxes, ID_NOfUnknown,
ID_InputFile, ID_OutputFile, ID_FileType,
ID_EpsFile, ID_PdfFile, ID_TifFile, ID_PngFile, ID_JpgFile, ID_MPFile,
ID_LoadedFilter, ID_RemappedKeys, ID_WrongFilterPath,
ID_Overfull, ID_Entries, ID_References, ID_ExtraProgram,
ID_PlugInInit, ID_PlugInReport, ID_HelpInfo, size_of_ID_list,
} Message_ID;


/*  These are the option variables */
struct texutilstruct {
  int UnknownOptions_field      ;
  int ProcessReferences_field   ;
  int ProcessIJ_field           ;
  int ProcessHigh_field         ;
  int ProcessQuotes_field       ;
  int ProcessDocuments_field    ;
  int ProcessSources_field      ;
  int ProcessSetups_field       ;
  int ProcessTemplates_field    ;
  int ProcessInfos_field        ;
  int ProcessFigures_field      ;
  int ProcessEpsPage_field      ;
  int ProcessEpsToPdf_field     ;
  int ProcessLogFile_field      ;
  int ProcessBox_field          ;
  int ProcessHBox_field         ;
  int ProcessVBox_field         ;
  int ProcessUnknown_field      ;
  int PurgeFiles_field          ;
  int PurgeAllFiles_field       ;
  int AnalyzeFile_field         ;
  int FilterPages_field         ;
  int SciteApi_field            ;
  int ProcessHelp_field         ;
  int ProcessSilent_field       ;
  int ProcessVerbose_field      ;
  char *TcXPath_field           ;
  char *ProcessType_field       ;
  char *ProcessOutputFile_field ;
  char *OutputFile_field        ;
  double ProcessCriterium_field ;
  char *UserInterface_field     ;
  char *InputFile_field         ;
  char *ProgramLog_field        ;  /* our default log file */
  FILE *STDOUT_field            ;
  FILE *TUO_field               ;
  int SortN_field               ;
  string *ExtraPrograms_field   ;
  int ExtraPrograms_size_field  ;
  int ExtraPrograms_add_field   ;
  int NOfExtraPrograms_field    ;
  string *RegisterEntry_field   ;
  int RegisterEntry_size_field  ;
  int RegisterEntry_add_field   ;
  string *SynonymEntry_field    ;
  int SynonymEntry_size_field   ;
  int SynonymEntry_add_field    ;
  string *Files_field           ;
  int *File_opens_field         ;
  int Files_size_field          ;
  int Files_add_field           ;
  int NOfFigures_field          ;
  string *Figures_field         ;
  int Figures_size_field        ;
  int Figures_add_field         ;
  int NOfCommands_field         ;
  int NOfSynonyms_field         ;
  int NOfBadSynonyms_field      ;
  int NOfEntries_field          ;
  int NOfBadEntries_field       ;
  int NOfFiles_field            ;
  int NOfBadFiles_field         ;
  int NOfPositionsFound_field   ;
  int TotalNOfPositions_field   ;
  int TotalNOfMPgraphics_field  ;
  const char *MS_field[size_of_ID_list];
};

typedef struct texutilstruct* texutil;

int     texutil_main (texutil TeXUtil, int ARGC,  string *ARGV);
texutil texutil_new (void);
