#include "ConstStore.h"
#include <stdlib.h>
void CONSTSTOREGet(int &n,conststore_t *store)
{
	if (store==NULL)
		return;
	if (store->type==CONSTSTORETYPEINT)
		n=store->data.i;
}
void CONSTSTOREGet(long int &n,conststore_t *store)
{
	if (store==NULL)
		return;
	if (store->type==CONSTSTORETYPELONGINT)
		n=store->data.li;
}
void CONSTSTOREGet(char &n,conststore_t *store)
{
	if (store==NULL)
		return;
	if (store->type==CONSTSTORETYPECHAR)
		n=store->data.c;
}
void CONSTSTOREGet(float &n,conststore_t *store)
{
	if (store==NULL)
		return;
	if (store->type==CONSTSTORETYPEFLOAT)
		n=store->data.f;
}
void CONSTSTOREGet(double &n,conststore_t *store)
{
	if (store==NULL)
		return;
	if (store->type==CONSTSTORETYPEDOUBLE)
		n=store->data.d;
}
void CONSTSTOREGet(long double &n,conststore_t *store)
{
	if (store==NULL)
		return;
	if (store->type==CONSTSTORETYPELONGDOUBLE)
		n=store->data.ld;
}

void CONSTSTORESet(int n,conststore_t *store)
{
	if (store->type==CONSTSTORETYPEINT)
		store->data.i=n;
}
void CONSTSTORESet(long int n,conststore_t *store)
{
	if (store->type==CONSTSTORETYPELONGINT)
		store->data.li=n;
}
void CONSTSTORESet(char n,conststore_t *store)
{
	if (store->type==CONSTSTORETYPECHAR)
		store->data.c=n;
}
void CONSTSTORESet(float n,conststore_t *store)
{
	if (store->type==CONSTSTORETYPEFLOAT)
		store->data.f=n;
}
void CONSTSTORESet(double n,conststore_t *store)
{
	if (store->type==CONSTSTORETYPEDOUBLE)
		store->data.d=n;
}
void CONSTSTORESet(long double n,conststore_t *store)
{
	if (store->type==CONSTSTORETYPELONGDOUBLE)
		store->data.ld=n;
}

void *CONSTSTORECreate(int n)
{
	conststore_t *newptr = NULL;
	newptr = new conststore_t;
	if (newptr==NULL)
		return NULL;
	newptr->type=CONSTSTORETYPEINT;
	newptr->data.i=n;
	return newptr;
}
void *CONSTSTORECreate(long int n)
{
	conststore_t *newptr = NULL;
	newptr = new conststore_t;
	if (newptr==NULL)
		return NULL;
	newptr->type=CONSTSTORETYPELONGINT;
	newptr->data.li=n;
	return newptr;
}
void *CONSTSTORECreate(char n)
{
	conststore_t *newptr = NULL;
	newptr = new conststore_t;
	if (newptr==NULL)
		return NULL;
	newptr->type=CONSTSTORETYPECHAR;
	newptr->data.c=n;
	return newptr;
}
void *CONSTSTORECreate(float n)
{
	conststore_t *newptr = NULL;
	newptr = new conststore_t;
	if (newptr==NULL)
		return NULL;
	newptr->type=CONSTSTORETYPEFLOAT;
	newptr->data.f=n;
	return newptr;
}
void *CONSTSTORECreate(double n)
{
	conststore_t *newptr = NULL;
	newptr = new conststore_t;
	if (newptr==NULL)
		return NULL;
	newptr->type=CONSTSTORETYPEDOUBLE;
	newptr->data.d=n;
	return newptr;
}
void *CONSTSTORECreate(long double n)
{
	conststore_t *newptr = NULL;
	newptr = new conststore_t;
	if (newptr==NULL)
		return NULL;
	newptr->type=CONSTSTORETYPELONGDOUBLE;
	newptr->data.ld=n;
	return newptr;
}

void CONSTSTOREDestroy(void *store)
{
	conststore_t *storeptr = (conststore_t *)store;
	delete storeptr;
}

void *CONSTSTOREloadDataI(int id)
{
	return NULL;
}
void *CONSTSTOREloadDataC(char*id)
{
	return NULL;
}