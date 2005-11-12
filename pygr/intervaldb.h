
#ifndef INTERVALDB_HEADER_INCLUDED
#define INTERVALDB_HEADER_INCLUDED 1
#include "default.h"

typedef struct {
  int start;
  int end;
  int target_id;
  int target_start;
  int target_end;
  int sublist;
} IntervalMap;


typedef struct {
  int start;
  int end;
} IntervalIndex;

typedef struct {
  int start;
  int len;
} SublistHeader;

typedef struct {
  int n;
  int ntop;
  int nlists;
  IntervalMap *im;
  SublistHeader *subheader;
} IntervalDB;

typedef struct {
  int n;
  int ntop;
  int nlists;
  int div;
  int nii;
  IntervalIndex *ii;
  SublistHeader *subheader;
  FILE *ifile_idb;
} IntervalDBFile;

typedef struct IntervalIterator_S {
  int i;
  int n;
  int nii;
  int ntop;
  int i_div;
  IntervalMap *im;
  struct IntervalIterator_S *up;
  struct IntervalIterator_S *down;
} IntervalIterator;


typedef struct {
  int id;
  int start;
  int stop;
} IDInterval;

extern int imstart_qsort_cmp(const void *void_a,const void *void_b);
extern IntervalMap *read_intervals(int n,FILE *ifile);
extern SublistHeader *build_nested_list(IntervalMap im[],int n,
					int *p_n,int *p_nlists);
extern IntervalMap *interval_map_alloc(int n);
extern IntervalDB *build_interval_db(IntervalMap im[],int n);
extern IntervalIterator *interval_iterator_alloc();
extern int free_interval_iterator(IntervalIterator *it);
extern IntervalIterator *reset_interval_iterator(IntervalIterator *it);
extern IntervalIterator *find_intervals(IntervalIterator *it0,int start,int end,IntervalMap im[],int n,SublistHeader subheader[],int nlists,IntervalMap buf[],int nbuf,int *p_nreturn);
extern int read_imdiv(FILE *ifile,IntervalMap imdiv[],int div,int i_div,int ntop);
extern IntervalMap *read_sublist(FILE *ifile,SublistHeader subheader[],int isub);
extern IntervalIterator *find_file_intervals(IntervalIterator *it0,int start,int end,
					     IntervalIndex ii[],int nii,
					     SublistHeader subheader[],int nlists,
					     int ntop,int div,FILE *ifile,
					     IntervalMap buf[],int nbuf,
					     int *p_nreturn);
extern char *write_binary_files(IntervalMap im[],int n,int ntop,int div,
				SublistHeader *subheader,int nlists,char filestem[]);
extern IntervalDBFile *read_binary_files(char filestem[],char err_msg[]);
extern int free_interval_dbfile(IntervalDBFile *db_file);
extern IDInterval *interval_id_alloc(int n);
extern int interval_id_union(int id,int start,int stop,IDInterval iv[],int n);
extern IDInterval *interval_id_compact(IDInterval iv[],int *p_n);

#define ITERATOR_STACK_TOP(it) while (it->up) it=it->up;
#define FREE_ITERATOR_STACK(it,it2,it_next) \
  for (it2=it->down;it2;it2=it_next) { \
    it_next=it2->down; \
    if (it2->im) \
      free(it2->im); \
    free(it2); \
  } \
  for (it2=it;it2;it2=it_next) { \
    it_next=it2->up; \
    if (it2->im) \
      free(it2->im); \
    free(it2); \
  }

#define PUSH_ITERATOR_STACK(it,it2,TYPE) \
  if (it->down) \
    it2=it->down; \
  else { \
    CALLOC(it2,1,TYPE); \
    it2->up = it; \
    it->down= it2; \
  }

/* IF it->up NON-NULL, MOVE UP, EVAL FALSE. 
   IF NULL, DON'T ASSIGN it, BUT EVAL TRUE */
#define POP_ITERATOR_STACK_DONE(it) (it->up==NULL || (it=it->up)==NULL)

#define POP_ITERATOR_STACK(it) (it->up && (it=it->up))

#define MERGE_INTERVAL_ORIENTATIONS 1
#ifdef MERGE_INTERVAL_ORIENTATIONS
#define START_POSITIVE(IM) (((IM).start>=0) ? ((IM).start) : -((IM).end))
#define END_POSITIVE(IM) (((IM).start>=0) ? ((IM).end) : -((IM).start))
#define SET_INTERVAL_POSITIVE(IM,START,END) if ((IM).start>=0) {\
  START= (IM).start; \
  END=   (IM).end; \
} else { \
  START= -((IM).end); \
  END=   -((IM).start); \
}

#define HAS_OVERLAP_POSITIVE(IM,START,END) (((IM).start>=0) ? \
    ((IM).start<(END) && (START)<(IM).end) \
  : (-((IM).end)<(END) && (START) < -((IM).start)))

#endif /* ????? MERGE_INTERVAL_ORIENTATIONS ??????? */


#endif