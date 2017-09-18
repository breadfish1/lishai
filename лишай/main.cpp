#include <iostream>
#include <stdlib.h>
#include <time.h>

#define SZ 21

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
    virtual healthy *NextStage(material &X) { return NULL; }
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
    // infected(int x, int y) { coord.x = x; coord.y = y; count = 6; } СПРОСИТЬ !!!
    infected(int x, int y) : immunity(x, y) { count = 6; }
    healthy *NextStage(material &X);
    int Type() const { return 2; }
protected:
    void Infect(material &X);
};

class material { // ГРАНИЦЫ В КОНСТРУКТОРЕ
public:
    material();
    ~material();
    healthy *GetCell(int x, int y) { return m[x][y]; }
    node *Clean(node *h);
    void Print() const;
    void Start();
    void TailCut(healthy *h);
    void BecomeInfected();
private:
    node *AddHead(node *head, healthy *_c); // добавить голову
    node *Delete(node *t, healthy *_c); // удалить элемент по значению
    healthy *m[SZ][SZ];
    node *head;
};

node *material::AddHead(node *head, healthy *_c) {
    node *p = new node;
    
    if (head) {
        p->h = _c;
        p->next = NULL;
        return p;
    }
    
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
    int lim = 8;
    while (lim > 0) {
        //coord.x
        //coord.y
    }
}

healthy *infected::NextStage(material &X) {
    Infect(X); // запускаем метод заражения соседних клеток
    count--; // отнимаем счетчик
    
    healthy *n = this;
    if (count == 0) { // если счетчик равен 0, то клетка становится имунной
        delete this;
        n = new immunity(coord.x, coord.y);
    }
    
    return n;
}

healthy *immunity::NextStage(material &X) {
    count--; // отнимаем счетчик
    
    healthy *n = this;
    if (count == 0) { // если равен нулю, то клетка становится здорвой, ура!!!
        delete this;
        n = new healthy(coord.x, coord.y);
    }
    
    return n;
}

void material::Print() const {
    int a;
    
    for(int i = 0; i < SZ; i++) {
        for(int j = 0; j < SZ; j++) {
            a = m[i][j]->Type();
            if (a == 1)
                cout << "🌝";
            if (a == 2)
                cout << "🌚";
            if (a == 3)
                cout << "🌎";
        }
        cout << endl;
    }
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
    
    for (int i = 0; i < SZ; i++)
        for (int j = 0; j < SZ; j++)
            m[i][j] = new healthy(i, j);
    
    delete m[n][n];
    
    m[n][n] = new infected(n, n); // центральная
    
    head = AddHead(head, m[n][n]);
}

material::~material() {
    head = Clean(head);
}

void material::TailCut(healthy *h) {
    if (head->h == h) { // если нужно удалить начиная с головы
        Clean(head);
        head = NULL;
        
        return;
    }
    
    
}

void material::Start() { // вот тут-то все и происходит
    node *temporary;
    healthy *e, *temp;
    
    while (head) { // идем по списку, пока он не окажется пуст
        temporary = head;
        while (temporary) { // делаем одну полную итерацию
            temporary->h = temporary->h->NextStage(*this);
            if (temporary->h->Type() == 1) {
                
                break;
            }
            temporary = temporary->next;
        }
        
    }
}

int main() {
    material Mat;
    Mat.Print();
    //Mat.Start();
}
