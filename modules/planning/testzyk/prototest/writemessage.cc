#include"test.pb.h"
#include<iostream>
#include<fstream>
#include<string>
using namespace std;

int main(int argc,char* argv[]){
    //@zyk:设置值
    tutorial::AddressBook book;

    tutorial::Person* person=book.add_people();

    person->set_name("zhangyankai");
    person->set_id(1);
    person->set_email("yankaizhang2020@163.com");

    tutorial::Person::PhoneNumber* phone=person->add_phones();
    phone->set_number("18713519612");
    phone->set_type(tutorial::Person_PhoneType_MOBILE);
    //@zyk:写入文件
    fstream output("./addressbook.pb.txt", ios::out | ios::trunc | ios::binary);
    if(!book.SerializeToOstream(&output)){
        cerr<<"failed to write"<<endl;
        return -1;
    }
    // Optional:  Delete all global objects allocated by libprotobuf.
    //@zyk:g++ writemessage.cc  test.pb.cc  -o writer -lprotobuf -pthread
}