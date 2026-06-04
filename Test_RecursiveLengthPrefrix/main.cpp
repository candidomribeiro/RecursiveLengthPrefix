#include "../recursivelengthprefix.h"
#include <iostream>

void teste();

int main()
{
    teste();
    return 0;
}

void teste()
{
    char ret [1024];
    char tst [1024];
    char ret1 [1024];
    char tst1 [1024];
    char retl [1024];
    char retl2[1024];

    const char* a = "韵文是讲究格律的，甚至大多数要使用同韵母的字作句字结尾，以求押韵的文体或文章。押韵是指在某些句子的最后一个字，都使用韵母相同或相近的字，使朗诵或咏唱时，产生铿锵和谐感。这些使用了同一韵母字的地方，称为韵脚。";
    const char* b = "Lorem ipsum dolor sit amet, consectetur adipisicing elit";

    std::cout << "\n\n第一个测试\n" << std::endl;

    RecursiveLengthPrefix::rlpEncode(ret, a);
    RecursiveLengthPrefix::rlpDecode(tst, ret);

    std::cout << tst << std::endl;

    std::cout << "\n\n第二个测试\n" << std::endl;

    RecursiveLengthPrefix::rlpEncode(ret1, b);
    RecursiveLengthPrefix::rlpDecode(tst1, ret1);

    std::cout << tst1 << std::endl;

    std::list<std::string> lst;
    std::list<std::string> lst2;
    std::list<std::string> retLst;
    std::list<std::string> retLst2;

    lst.push_back("vamos ");
    lst.push_back("testar ");
    lst.push_back("isso ");
    lst.push_back("aqui ");
    lst.push_back("para ");
    lst.push_back("ver ");
    lst.push_back("se ");
    lst.push_back("funciona ");

    std::cout << "\n\n第三个测试\n" << std::endl;
    RecursiveLengthPrefix::rlpEncode(retl, &lst);
    RecursiveLengthPrefix::rlpDecode(&retLst, retl);

    for (const auto &item : retLst)
    {
        std::cout << item << std::endl;
    }

    std::cout << "\n\n第四个测试\n" << std::endl;

    lst2.push_back(a);
    lst2.push_back(retl); // RLP String
    lst2.push_back(b);
    lst2.push_back("A");

    RecursiveLengthPrefix::rlpEncode(retl2, &lst2);
    RecursiveLengthPrefix::rlpDecode(&retLst2, retl2);

    for (const auto &item2 : retLst2)
    {
        std::cout << item2 << std::endl;
    }
}
