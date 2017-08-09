
#ifndef KPATHSEA_TYPES_H
typedef char *string;
#endif

extern int EnterMode;
#define Batch 1
#define NonStop 2

extern char *Format;
extern char *TeXFormatFlag;
extern char *TeXBatchFlag;
extern char *TeXNonStopFlag;
extern int ForceXML;

extern int SystemTeX (char *JobFile) ;
extern int texexec_main(int ac,  string *av);
