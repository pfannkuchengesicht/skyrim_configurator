#ifndef CLIST_H
#define CLIST_H
class CListItem
{
private:
    CListItem *pPrev;
    CListItem *pNext;
    void *pData;
public:
    CListItem();
    CListItem(CListItem *pPrev, CListItem *pNext, void *pData);
    CListItem(CListItem *pNext, void *pData, bool prev);
    CListItem(void *pData);
    void setPrev(CListItem *pPrev);
    void setNext(CListItem *pNext);
    void setData(void *pData);
    void* getData();
    CListItem* getNext();
    CListItem* getPrev();
    void destroy(void (*del)(void*));
};

class CList
{
private:
    CListItem *pFirst;
    CListItem *pLast;
    CListItem *pCur;
public:
    CList();
    void addFirst(void* pData);
    void addLast(void* pData);
    void addBefore(void* pData);
    void addAfter(void* pData);
    void add(void* pData, int (*cmp)(void*, void*));
    void* getFirst();
    void* getLast();
    void* getNext();
    void* getPrev();
    void* getCur();
    void destroy(void (*del)(void*));
};
#endif // CLIST_H
