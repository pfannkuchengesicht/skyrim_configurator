#include <stdlib.h>
#include "CList.h"

CListItem::CListItem()
{
    this->pPrev = NULL;
    this->pNext = NULL;
    this->pData = NULL;
}

CListItem::CListItem(CListItem *pPrev, CListItem *pNext, void *pData)
{
    this->pPrev = pPrev;
    this->pNext = pNext;
    this->pData = pData;
}

CListItem::CListItem(CListItem *pItem, void *pData, bool prev)
{
    if(prev)
    {
        this->pPrev = pItem;
        this->pNext = NULL;
    }
    else
    {
        this->pPrev = NULL;
        this->pNext = pItem;
    }

    this->pData = pData;
}

CListItem::CListItem(void *pData)
{
    this->pPrev = NULL;
    this->pNext = NULL;
    this->pData = pData;
}

void CListItem::setPrev(CListItem *pPrev)
{
    this->pPrev = pPrev;
}

void CListItem::setNext(CListItem *pNext)
{
    this->pNext = pNext;
}

void CListItem::setData(void *pData)
{
    this->pData = pData;
}

void *CListItem::getData()
{
    return pData;
}

CListItem *CListItem::getNext()
{
    return pNext;
}

CListItem *CListItem::getPrev()
{
    return pPrev;
}

void CListItem::destroy(void (*del)(void *))
{
    del(pData);
    delete this;
}

CList::CList()
{
    this->pCur = NULL;
    this->pFirst = NULL;
    this->pLast = NULL;
}

void CList::addFirst(void *pData)
{
    CListItem *tmp = new CListItem(pData);

    if(!tmp) return;

    tmp->setPrev(NULL);

    if(!pFirst)
    {
        tmp->setNext(NULL);
        pFirst = tmp;

        if(!pLast)
        {
            pLast = tmp;
        }
    }
    else
    {
        tmp->setNext(pFirst);
        pFirst->setPrev(tmp);
        pFirst = tmp;
    }

    pCur = tmp;
}

void CList::addLast(void *pData)
{
    CListItem *tmp = new CListItem(pData);

    if(!tmp) return;

    tmp->setNext(NULL);

    if(!pLast)
    {
        tmp->setPrev(NULL);
        pLast = tmp;

        if(!pFirst)
        {
            pFirst = tmp;
        }
    }
    else
    {
        tmp->setPrev(pLast);
        pLast->setNext(tmp);
        pLast = tmp;
    }

    pCur = tmp;
}

void CList::addBefore(void *pData)
{
    if(pCur)
    {
        CListItem *tmp = new CListItem(pData);

        if(!tmp) return;

        if(pCur->getPrev())
        {
            tmp->setPrev(pCur->getPrev());
            pCur->getPrev()->setNext(tmp);
        }
        else
        {
            pFirst = tmp;
        }

        tmp->setNext(pCur);
        pCur->setPrev(tmp);
        pCur = tmp;
    }
    else
    {
        addLast(pData);
    }
}

void CList::addAfter(void *pData)
{
    if(pCur)
    {
        CListItem *tmp = new CListItem(pData);

        if(!tmp) return;

        if(pCur->getNext())
        {
            tmp->setNext(pCur->getNext());
            pCur->getNext()->setPrev(tmp);
        }
        else
        {
            pLast = tmp;
        }

        tmp->setPrev(pCur);
        pCur->setNext(tmp);
        pCur = tmp;
    }
    else
    {
        addLast(pData);
    }
}

void CList::add(void *pData, int (*cmp)(void *, void *))
{
    CListItem *tmp = new CListItem(pData);

    if(!tmp) return;

    void* i;
    for(i = getFirst(); i; i = getNext())
    {
        if(cmp(i, pData) > 0)
        {
            addAfter(pData);
            return;
        }
    }

    addLast(pData);
}

void *CList::getFirst()
{
    return pFirst->getData();
}

void *CList::getLast()
{
    return pLast->getData();
}

void *CList::getNext()
{
    if(pCur)
    {
        if(pCur->getNext())
        {
            pCur = pCur->getNext();
            return pCur->getData();
        }

        return NULL;
    }

    return NULL;
}

void *CList::getPrev()
{
    if(pCur)
    {
        if(pCur->getPrev())
        {
            pCur = pCur->getPrev();
            return pCur->getData();
        }

        return NULL;
    }

    return NULL;
}

void *CList::getCur()
{
    return pCur;
}

void CList::destroy(void (*del)(void *))
{
    void *i;
    for(i = getFirst(); i; i = getNext())
    {
        del(i);
    }

    delete this;
}
