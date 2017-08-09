
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "texutil.h"

typedef struct option getoptstruct;


static struct texutilstruct the_texutil = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                           "","","",NULL,0.0,"en",NULL,"texutil.log",NULL,NULL,0,
										   NULL,10,10,0,NULL,100,100,NULL,100,100,NULL,NULL,10,10,0,
                                           NULL,100,100,0,0,0,0,0,0,0,0,0,0,{NULL}};

#define STREQ(s1, s2) ((s1) && (strcmp (s1, s2) == 0))
#define ARGUMENT_IS(a) STREQ (long_options[option_index].name, a)

int parse_options(texutil TeXUtil,int argc,  string * argv) {
  int g;   /* `getopt' return code.  */
  int option_index;
  struct option long_options[]
    = { { "references", 0, &TeXUtil->ProcessReferences_field, 1 },
	{ "ij",         0, &TeXUtil->ProcessIJ_field,         1 },
	{ "high",       0, &TeXUtil->ProcessHigh_field,       1 },
	{ "quotes",     0, &TeXUtil->ProcessQuotes_field,     1 },
	{ "tcxpath",    1, 0,                  0 }, // TcXPath
	{ "documents",  0, &TeXUtil->ProcessDocuments_field,  1 },
	{ "type",       1, 0,                  0 }, // ProcessType
	{ "outputfile", 1, 0,                  0 }, // ProcessOutputFile,
	{ "sources",    0, &TeXUtil->ProcessSources_field,    1 },
	{ "setups",     0, &TeXUtil->ProcessSetups_field,     1 },
	{ "templates",  0, &TeXUtil->ProcessTemplates_field,  1 },
	{ "infos",      0, &TeXUtil->ProcessInfos_field,      1 },
	{ "figures",    0, &TeXUtil->ProcessFigures_field,    1 },
	{ "epspage",    0, &TeXUtil->ProcessEpsPage_field,    1 },
	{ "epstopdf",   0, &TeXUtil->ProcessEpsToPdf_field,   1 },
	{ "logfile",    0, &TeXUtil->ProcessLogFile_field,    1 },
	{ "box",        0, &TeXUtil->ProcessBox_field,        1 },
	{ "hbox",       0, &TeXUtil->ProcessHBox_field,       1 },
	{ "vbox",       0, &TeXUtil->ProcessVBox_field,       1 },
	{ "criterium",  1, 0,                  0 }, // ProcessCriterium,
	{ "unknown",    0, &TeXUtil->ProcessUnknown_field,    1 },
	{ "purge",      0, &TeXUtil->PurgeFiles_field,        1 },
	{ "purgeall",   0, &TeXUtil->PurgeAllFiles_field,     1 },
	{ "analyze",    0, &TeXUtil->AnalyzeFile_field,       1 },
	{ "filter",     0, &TeXUtil->FilterPages_field,       1 },
	{ "sciteapi",   0, &TeXUtil->SciteApi_field,          1 },
	{ "help",       0, &TeXUtil->ProcessHelp_field,       1 },
	{ "silent",     0, &TeXUtil->ProcessSilent_field,     1 },
	{ "verbose",    0, &TeXUtil->ProcessVerbose_field,    1 },
	{ "interface",  1, 0,                  0 },  // UserInterface
	{ 0, 0, 0, 0 } };
  opterr = 0;
  for (;;) {
    g = getopt_long (argc, argv, "+", long_options, &option_index);
    if (g == -1) /* End of arguments, exit the loop.  */
      break;
    if (g == '?') { /* Unknown option.  */
      TeXUtil->UnknownOptions_field = 1;
      break;
    }
    if (ARGUMENT_IS ("tcxpath")) {
      TeXUtil->TcXPath_field = optarg;
    } else if (ARGUMENT_IS ("type")) {
      TeXUtil->ProcessType_field = optarg;
    } else if (ARGUMENT_IS ("outputfile")) {
      TeXUtil->ProcessOutputFile_field = optarg;
    } else if (ARGUMENT_IS ("interface")) {
      TeXUtil->UserInterface_field = optarg;
    } else if (ARGUMENT_IS ("criterium")) {
      TeXUtil->ProcessCriterium_field = atof (optarg);
    } /* Else it was a flag; getopt has already done the assignment.  */
  }
  return optind;
}



int main(int ac,  string *av) {
  texutil TeXUtil = &the_texutil;
  TeXUtil->STDOUT_field = stdout;
  int optind;
  optind = parse_options (TeXUtil, ac,av);
  int ARGC = (ac-optind);
  string *ARGV = av+optind;
  return texutil_main(TeXUtil,ARGC,ARGV);
}
