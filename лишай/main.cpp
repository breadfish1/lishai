#include <iostream>
#include <stdlib.h>
#include <time.h>

#define SZ 21

using namespace std;

class cell;

class coordinate {
public:
    int x, y;
};

class node {
public:
    cell **c;
    node *next;
};

class cell {
public:
    virtual int Type() const = 0;
    virtual cell *NextStage() = 0;
    coordinate GetCoord();
protected:
    coordinate coord;
};

class healthy: public cell {
public:
    healthy(int x, int y) { coord.x = x; coord.y = y; }
    cell *NextStage() {return NULL;}
    int Type() const { return 1; }
};

class infected: public cell {
public:
    infected(int x, int y) { coord.x = x; coord.y = y; count = 6; }
    cell *NextStage();
    int Type() const { return 2; }
protected:
    int count;
};

class immunity: public cell {
public:
    immunity(int x, int y) { coord.x = x; coord.y = y; count = 4; }
    cell *NextStage();
    int Type() const { return 3; }
protected:
    int count;
};

class material {
public:
    material();
    ~material();
    cell *GetCell(int x, int y) { return m[x][y]; }
    node *Clean(node *head);
    
    
private:
    node *AddHead(node *head, cell **_c); // добавить голову
    node *Delete(node *t, cell **_c); // удалить элемент по значению
    cell *m[SZ][SZ];
    node *head;
};

node *material::AddHead(node *head, cell **_c) {
    node *p = new node;
    
    if (head) {
        p->c = _c;
        p->next = NULL;
        return p;
    }
    
    p->c = _c;
    p->next = head;
    return p;
}

node *material::Delete(node *t, cell **_c) {
    node *p = t;
    
    if (t->c == _c) { // если это голова
        p = t->next;
        delete t;
        return p;
    }

    while (p->next->c != _c) // ищем элемент, стоящий до удаляемого
        p = p->next;
    
    node *q = p->next->next;
    delete p->next;
    p->next = q;
    return t;
}

material::material() {
    int n = SZ/2;
    
    for (int i = 0; i <= n; i++)
        for (int j = 0; j < n; j++)
            m[i][j] = new healthy(i, j);
    
    m[n][n] = new infected(n, n);
    
    for (int j = n + 1; j < SZ; j++)
        m[n][j] = new healthy(n, j);
    
    for (int i = n + 1; i < SZ; i++)
        for (int j = 0; j < SZ; j++)
            m[i][j] = new healthy(i, j);
        
}
