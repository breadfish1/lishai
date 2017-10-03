#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>

#define SZ 21

// ПЛЮС БОРДЮРЧИКС

using namespace std;

class healthy;
class material;

class coordinate {
public:
    int x, y;
};

class node {
public:
    healthy *h;
    node *next;
};

class healthy {
public:
    healthy(int x, int y) { coord.x = x; coord.y = y; }
    virtual healthy *NextStage(material &X) { return this; }
    virtual int Type() const { return 1; }
protected:
    coordinate coord;
};

class immunity: public healthy {
public:
    immunity(int x, int y) : healthy(x, y) { count = 4; }
    healthy *NextStage(material &X);
    int Type() const { return 3; }
protected:
    int count;
};

class infected: public immunity {
public:
    // infected(int x, int y) { coord.x = x; coord.y = y; count = 6; }
    infected(int x, int y) : immunity(x, y) { count = 6; }
    healthy *NextStage(material &X);
    int Type() const { return 2; }
protected:
    void Infect(material &X);
};

class material {
public:
    material();
    ~material();
    healthy *GetCell(int x, int y) { return m[x][y]; }
    node *Clean(node *h);
    void Print() const;
    void Start();
    node *TailCut(healthy *h);
    void BecomeInfected(int x, int y);
    void EditHealthy(int x, int y, healthy *h);
private:
    node *AddHead(node *head, healthy *_c); // добавить голову
    node *Delete(node *t, healthy *_c); // удалить элемент по значению
    healthy *m[SZ][SZ];
    node *head;
};

void material::EditHealthy(int x, int y, healthy *h) {
    m[x][y] = h;
}

void material::BecomeInfected(int x, int y) {
    delete m[x][y];
    m[x][y] = new infected(x, y);
    head = AddHead(head, m[x][y]);
}

node *material::AddHead(node *head, healthy *_c) {
    node *p = new node;
    
    p->h = _c;
    p->next = head;
    return p;
}

node *material::Delete(node *t, healthy *_c) {
    node *p = t;
    
    if (t->h == _c) { // если это голова
        p = t->next;
        delete t;
        return p;
    }

    while (p->next->h != _c) // ищем элемент, стоящий до удаляемого
        p = p->next;
    
    node *q = p->next->next;
    delete p->next;
    p->next = q;
    return t;
}

void infected::Infect(material &X) {
    int r;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            healthy *a = X.GetCell(coord.x + i, coord.y + j);
            r = rand()%2;
            if ((i || j) && a != NULL && a->Type() == 1 && r) {
                X.BecomeInfected(coord.x + i, coord.y + j);
                return;
            }
        }
    }
}

healthy *infected::NextStage(material &X) {
    Infect(X); // запускаем метод заражения соседних клеток
    count--; // отнимаем счетчик
    
    healthy *n = this;
    if (count == 0) { // если счетчик равен 0, то клетка становится имунной
        delete n;
        n = new immunity(coord.x, coord.y);
    }
    
    X.EditHealthy(coord.x, coord.y, n);
    
    return n;
}

healthy *immunity::NextStage(material &X) { // ПЕРЕД ВЫЗОВОМ СОХРАНЯЕМ СТАРЫЙ УКАЗАТЕЛЬ, ВОЗВРАЩАЕМ this ИЛИ new healthy, СРАВНИВАЕМ С СОХРАНЕННЫМ, ЕСЛИ НУЖНО, ТО ПЕРЕПРИСВАИВАЕМ...
    count--; // отнимаем счетчик
    
    healthy *n = this;
    if (count == 0) { // если равен нулю, то клетка становится здорвой, ура!!!
        delete n;
        n = new healthy(coord.x, coord.y);
    }
    
    X.EditHealthy(coord.x, coord.y, n);
    
    return n;
}

void material::Print() const {
    int a;

    node *temp = head;
    
//    while (temp) {
//        a = temp->h->Type();
//        if (a == 1)
//            cout << "🌝" << " -> ";
//        if (a == 2)
//            cout << "🌚" << " -> ";
//        if (a == 3)
//            cout << "🌎" << " -> ";
//        temp = temp->next;
//    }
//
//    cout << endl << endl;

    system("clear");
    
    for(int i = 0; i < SZ; i++) {
        for(int j = 0; j < SZ; j++) {
            a = m[i][j]->Type();
            if (a == 1)
                //cout << ".";
                cout << "🌝";
            if (a == 2)
                //cout << "X";
                cout << "🌚";
            if (a == 3)
                //cout << "-";
                cout << "🌎";
        }
        cout << endl;
    }
    cout << endl;
}

node *material::Clean(node *h) {
    if (h != NULL) {
        node *temp = h;
        while (temp != NULL) {
            h = temp->next;
            delete temp;
            temp = h;
        }
    }
    
    return NULL;
}

material::material() {
    int n = SZ/2;
    head = NULL;
    
    for (int i = 1; i < SZ - 1; i++)
        for (int j = 1; j < SZ - 1; j++)
            m[i][j] = new healthy(i, j);
    
    delete m[n][n];
    
    m[n][n] = new infected(n, n); // центральная
    
    int s = SZ - 1;
    for (int i = 0; i < SZ; i++) {
        m[i][0] = new infected(i, 0);
        m[0][i] = new infected(0, i);
        m[s][i] = new infected(s, i);
        m[i][s] = new infected(i, s);
    }
    
    head = AddHead(head, m[n][n]);
}

material::~material() {
    head = Clean(head);
}

node *material::TailCut(healthy *h) {
    if (head->h == h) {
        Clean(head);
        head = NULL;
    }
    else {
        node *p = head;
        node *t = p->next;
        while(t->h != h) {
            p = t;
            t = p->next;
        }

        p->next = NULL;
        Clean(t);
    }
    
    return head;
}

void material::Start() { // вот тут-то все и происходит
    node *temporary, *mem = NULL;

    while (head) { // идем по списку, пока он не окажется пуст
        temporary = head;
        while (temporary) { // делаем одну полную итерацию
            temporary->h = temporary->h->NextStage(*this);
            if (temporary->h->Type() == 1) // ОСТАЛЬНЫЕ ТОЖЕ НУЖНО СДЕЛАТЬ ЗДОРОВЫМИ
                break;
            temporary = temporary->next;
        }
        
        mem = temporary;
        while (temporary) {
            temporary->h = temporary->h->NextStage(*this);
            temporary = temporary->next;
        }
        
        if (mem != NULL)
            head = TailCut(mem->h); // ??
        Print();
    }
}

int main() {
    material Mat;
    Mat.Print();
    Mat.Start();
}
