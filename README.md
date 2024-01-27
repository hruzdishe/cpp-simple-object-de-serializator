```
int main() {
    Serializator s;

    Person p{"Bob", 20, 15.51};
    s.serialize<Person>(&p);
    /*
    * in file 
    * 
    * int age 20
    * float balance 15.51
    * string name Bob
    */

    s.deserialize<Person>().print(); // OUTPUT: Bob 20 15.51
}
```
