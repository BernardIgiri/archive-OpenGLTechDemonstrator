/***************************************************************************

  Include ONDDAS prior to including this to enable ONDDAS support
  macros.

***************************************************************************/
#ifndef __INCLUDED_CONSTSTORE__
#define __INCLUDED_CONSTSTORE__
#define CONSTSTORETYPEINT		 1
#define CONSTSTORETYPELONGINT	 2
#define CONSTSTORETYPECHAR		 3
#define CONSTSTORETYPEFLOAT		 4
#define CONSTSTORETYPEDOUBLE	 5
#define CONSTSTORETYPELONGDOUBLE 6
typedef struct
{
	union {
		int			i;
		long int	li;
		char		c;
		float		f;
		double		d;
		long double ld;
	} data;
	int type;
} conststore_t;
#define CONSTSTOREGET(store,data) CONSTSTOREGet(data,(conststore_t *)store)
#define CONSTSTORESET(store,data) CONSTSTORESet(data,&store)
#define CONSTSTORECREATE(data)	  CONSTSTORECreate(data)
#define CONSTSTOREDESTROY		  CONSTSTOREDestroy
#ifdef __INCLUDED_ONDEMANDDATAACCESSSYSTEM__
#define CONSTSTOREINITI(onddasPtr) (onddasPtr)->SetConstant((onddasPtr)->StartNewTree((onddasPtr)->RegisterDataType(CONSTSTOREloadDataI,CONSTSTOREDESTROY)))
#define CONSTSTOREINITC(onddasPtr) (onddasPtr)->SetConstant((onddasPtr)->StartNewTree((onddasPtr)->RegisterDataType(CONSTSTOREloadDataC,CONSTSTOREDESTROY)))
#define CONSTSTOREGETCONST(onddasPtr,id,retval) CONSTSTOREGET((onddasPtr)->AccessData(id,(onddasPtr)->GetConstant()),retval)
#define CONSTSTOREINSERT(onddasPtr,id,data) (onddasPtr)->InsertData(id,(onddasPtr)->GetConstant(),CONSTSTORECREATE(data));
#endif
void CONSTSTOREGet(int &n,conststore_t *store);
void CONSTSTOREGet(long int &n,conststore_t *store);
void CONSTSTOREGet(char &n,conststore_t *store);
void CONSTSTOREGet(float &n,conststore_t *store);
void CONSTSTOREGet(double &n,conststore_t *store);
void CONSTSTOREGet(long double &n,conststore_t *store);
void CONSTSTORESet(int n,conststore_t *store);
void CONSTSTORESet(long int n,conststore_t *store);
void CONSTSTORESet(char n,conststore_t *store);
void CONSTSTORESet(float n,conststore_t *store);
void CONSTSTORESet(double n,conststore_t *store);
void CONSTSTORESet(long double n,conststore_t *store);
void *CONSTSTORECreate(int n);
void *CONSTSTORECreate(long int n);
void *CONSTSTORECreate(char n);
void *CONSTSTORECreate(float n);
void *CONSTSTORECreate(double n);
void *CONSTSTORECreate(long double n);
void CONSTSTOREDestroy(void *store);
void *CONSTSTOREloadDataI(int id);
void *CONSTSTOREloadDataC(char*id);
#endif
