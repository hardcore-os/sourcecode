#include<pthread.h>




class Person {
    public:
    void Print(int a) {

    }

    void Print(int a, int b) {

    }
};

void*Test1(void* para) {
PersonWrapperOneParameter *pw = static_cast<PersonWrapperOneParameter*>(para);
pw->p->Print(pw->a);
}


struct PersonWrapperOneParameter {
    Person *p;
    int a;
};

struct PersonWrapperTwoParameter {
    Person *p;
    int a;
    int b;
};

void*Test2(void* para) {
PersonWrapperTwoParameter *pw = static_cast<PersonWrapperTwoParameter*>(para);
pw->p->Print(pw->a,pw->b);
}


int main() {
    pthread_t pt;
    // 一个参数的
    PersonWrapperOneParameter *pw = new PersonWrapperOneParameter;
    pw->p =new Person;
    pw->a = 12;
    pthread_create(&pt,NULL,Test1, (void*)pw);


// 一个参数的
    PersonWrapperTwoParameter *pw2 = new PersonWrapperTwoParameter;
    pw2->p =new Person;
    pw2->a = 12;
    pw2->b = 12;
    pthread_create(&pt,NULL,Test2, (void*)pw2);


}