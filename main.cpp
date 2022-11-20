#include <iostream>
#include <vector>
#include <map>
#include <string>


struct Node
{
    char sign;
    bool eom = false;
    unsigned long long int frequency = 0;

    Node* Left = nullptr;
    Node* Right = nullptr;
};

class Huffman
{
    public:
    Node* Root;

    //map of codes for signs (key -> character, value -> code for it)
    std::map <char, std::string> codeMap;
    std::string eomCode; // contains code for End Of Message
    std::vector <unsigned char> encodedMessage;

    //vectors beneath are ment to speed process of calculations up. Is it right idea?
    std::vector <char>  characterVector; //contains characters used in message
    std::vector <long long unsigned int> frequencyVector; //contains frequences of characters

    Huffman();
    void countCharacters(std::string message);  //clears CharacterVector and frequencyVector and fills them according to new data
    void printVectors(); // just for debuging
    void createTree();
    void clearTree(Node** curr);
    void fillCodeMap();
    void recurFindPath(Node* curr, std::vector <bool> incompletepath, bool prev);
    void returnPath(char sign, std::vector <bool> completePath);
    void returnPath(std::vector <bool> completePath);   //call when EOM node
    void printMap();
    void compress(std::string primal);
    void encode(std::vector <unsigned char>& encodedMessage, int& spaceLeft, std::string toEncode);
    void printEncodedMessage();
    std::string decode();
    void deleteTree(Node* node);
};

    int power(int x, int y)
    {
        if(y == 0)
            return 1;

        return 2*power(x,y-1);
    }


    //Converts LAST 8 elements of string to bites of unsigned char
    unsigned char stringToUnsChar(std::string toConvert)
    {
            unsigned char result = 0b00000000;

            for(int i=int(toConvert.length()-1); i>=0; i--)
               {
                    if(char(toConvert[i])=='1')
                    result += power(2,int(toConvert.length())-i-1);
               }
            return result;
    }

    Huffman::Huffman()
    {
        Root = nullptr;
    }

    //clears tree where curr is a root
    void Huffman::clearTree(Node** curr)
    {
        deleteTree(*curr);
        *curr = nullptr;
    }

    //deletes all nodes of tree (Root included) where curr is a root
    void Huffman::deleteTree(Node *curr)
    {
        if(curr == nullptr)
            return;

        deleteTree(curr->Left);
        deleteTree(curr->Right);

        delete curr;
    }

    void Huffman::countCharacters(std::string message)
    {
       characterVector.clear();
       frequencyVector.clear();

       for(int i=0; i<int(message.length()); i++)
       {
           //try to find same char in chartable and increment frequency for the character
           bool foundChar = false;
           for(int j=0; j<int(characterVector.size());j++)
           {
               if(message[i] == characterVector[j])
               {
                   frequencyVector[j]++;
                   foundChar = true;
                   break;
               }
           }
           //if such a character hasn`t been add to table yet do it now with frequency 1
           if(!foundChar)
           {
               frequencyVector.push_back(1);
               characterVector.push_back(message[i]);
           }
       }
    }

    void Huffman::printVectors()
    {
        for(int i=0; i<int(characterVector.size()); i++)
        {
            std::cout<<characterVector[i]<<"    "<<frequencyVector[i]<<std::endl;
        }
    }

    void Huffman::createTree()
    {
       std::vector <Node*> NodeVector;
       std::vector <long long unsigned int> localFrequency = frequencyVector;
       clearTree(&Root);

       for(int i =0; i<int(characterVector.size());i++)
       {
           Node* temp = new Node;
           temp->frequency = frequencyVector[i];
           temp->sign = characterVector[i];
           temp->eom = false;
           NodeVector.push_back(temp);
       }
       //add EOM (end of Message) node
       //EOM node is not counted in characterVector!
           Node* temp = new Node;
           temp->frequency = 1;
           temp->eom = true;
           temp->sign = '*';
           NodeVector.push_back(temp);
           localFrequency.push_back(1);


           while(NodeVector.size()>1)
           {
               long long unsigned int min1,min2;
               int index1, index2;

               min1 = localFrequency[0];
               index1 = 0;

                //find minimum frequency node with index index1 and frequency min1
                for(int i=1; i<int(localFrequency.size()); i++)
                {
                    if(localFrequency[i]<min1)
                    {
                        index1 = i;
                        min1 = localFrequency[i];
                    }
                }

                if(index1 == 0)
                {
                    index2=1;
                    min2 = localFrequency[1];
                }
                else
                {
                    index2=0;
                    min2 = localFrequency[0];
                }

                //finds 2nd minimum frequency
                for(int i=0; i<int(localFrequency.size()); i++)
                {
                    if(localFrequency[i]<min2 && i != index1)
                    {
                        index2 = i;
                        min2 = localFrequency[i];
                    }
                }

                Node* temp = new Node;
                temp->Left = NodeVector[index2];
                temp->Right = NodeVector[index1];
                temp->frequency = min1+min2;

                if(index1 > index2)
                {
                    NodeVector.erase(NodeVector.begin()+index1);
                    NodeVector.erase(NodeVector.begin()+index2);

                    localFrequency.erase(localFrequency.begin()+index1);
                    localFrequency.erase(localFrequency.begin()+index2);
                }

                if(index2 > index1)
                {
                    NodeVector.erase(NodeVector.begin()+index2);
                    NodeVector.erase(NodeVector.begin()+index1);
                    localFrequency.erase(localFrequency.begin()+index2);
                    localFrequency.erase(localFrequency.begin()+index1);
                }

                NodeVector.push_back(temp);
                localFrequency.push_back(temp->frequency);

           }
           Root = NodeVector[0];

    }

    void Huffman::returnPath(char sign, std::vector <bool> completePath)
    {
        std::string temp ="";
        for(int i=0; i<int(completePath.size());i++)
        {
            if(completePath[i])
                temp+='1';
            else
                temp+='0';
        } //temp is now chain of 0 and 1 and that`s the path to a node

        codeMap[sign] = temp;
    }

    void Huffman::returnPath(std::vector <bool> completePath)
    {
        std::string temp ="";
        for(int i=0; i<int(completePath.size());i++)
        {
            if(completePath[i])
                temp+='1';
            else
                temp+='0';
        } //temp is now chain of 0 and 1 and that`s the path to a node

        eomCode = temp;
    }

    void Huffman::recurFindPath(Node* curr, std::vector <bool> incompletepath, bool prev)
    {
        //finnished this branch
        if(curr == nullptr)
            return;

        incompletepath.push_back(prev);

        //its leave - path to character has been found
        if(curr->Left==nullptr && curr->Right == nullptr)
        {
            if(curr->eom)
            returnPath(incompletepath);
            else
            returnPath(curr->sign,incompletepath);
        }
        //if not go both ways down
        else
        {
            recurFindPath(curr->Left,incompletepath,0);
            recurFindPath(curr->Right,incompletepath,1);
        }
    }

    void Huffman::fillCodeMap()
    {
        createTree();
        std::vector <bool> temp;

        recurFindPath(Root->Left,temp,0);
        recurFindPath(Root->Right,temp,1);
    }

    void Huffman::printMap()
    {
        std::map <char, std::string>::iterator iter;
            for(iter=codeMap.begin(); iter != codeMap.end() ;iter++)
            {
                std::cout<<iter->first<<" "<<iter->second<<std::endl;
            }
    }

    void Huffman::printEncodedMessage()
    {
        std::vector <unsigned char>::iterator iter;

        for(iter = encodedMessage.begin(); iter != encodedMessage.end(); iter++)
        {
            unsigned char temp = *iter;
                for(int i=0; i<8; i++)
                {
                  std::cout<<(temp&0b10000000)/power(2,7);
                  temp = temp<<1;
                }
        }
    }

    //encodedMessage -> vector which will be filled with encoded bits
    //to Encode -> chain of 0 and 1 to put in bites
    void Huffman::encode(std::vector <unsigned char>& encodedMessage, int& spaceLeft, std::string toEncode)
    {
        if(int(toEncode.length()) < spaceLeft)
        {
           unsigned char temp = (encodedMessage.back()<<int(toEncode.length()));
           spaceLeft -= int(toEncode.length());

           temp += stringToUnsChar(toEncode);
           encodedMessage.back() = temp;

           if(spaceLeft == 0)
           {
               encodedMessage.push_back(0b00000000);
               spaceLeft = 8;
           }
           return;
        }

        else
        {
            std::string _toEncode = toEncode.substr(spaceLeft,size_t(toEncode.length()-spaceLeft));
            std::string encodeNow = toEncode.substr(0,size_t(spaceLeft));

            unsigned char temp = (encodedMessage.back()<<spaceLeft);
            temp+=stringToUnsChar(encodeNow);
            encodedMessage.back() = temp;

            encodedMessage.push_back(0b00000000);
            spaceLeft = 8;
            encode(encodedMessage,spaceLeft,_toEncode);

        }


    }

    void Huffman::compress(std::string primal)
    {
        countCharacters(primal);
        fillCodeMap();
        encodedMessage.clear();
        encodedMessage.push_back(0b00000000);
        int spaceLeft = 8;

        for(int i=0; i < int(primal.length()); i++)
        {
            std::string temp = codeMap[primal[i]];
            encode(encodedMessage,spaceLeft,temp);
        }
        encode(encodedMessage,spaceLeft,eomCode);
        //fix last byte
         unsigned char temp = (encodedMessage.back()<<spaceLeft);
         encodedMessage.back() = temp;
    }

    //work in progress
    std::string Huffman::decode()
    {
        std::string decodedMessage = "";
        Node* ptr = Root;

        for(int j = 0;j<int (encodedMessage.size()); j++)
        {

            unsigned char temp = encodedMessage[j];

                for(int i=0; i<8; i++)
                {
                    //if there`s 1 on firs bit
                   if((temp & 0b10000000) == 0b10000000)
                   {
                       ptr = ptr->Right;
                   }
                   else
                   {
                       ptr = ptr->Left;
                   }

                   temp = (temp<<1); //shift left

                   //this means it`s a leave
                   if(ptr->Left == nullptr && ptr->Right == nullptr)
                   {
                       if(ptr->eom)
                       {
                           return decodedMessage;
                       }
                       decodedMessage += ptr -> sign;
                       ptr = Root;
                   }
                }

        }
        return "ERROR: EOM code not found.";
    }

int main()
{
    std::string primalText = "abecargktlngvewtgwtrhnytyhwntrasegwbergw[wietrjg[oviwrjtgo[wrjga spadlo";

    Huffman object;

    object.compress(primalText);


    std::cout<<std::endl;

    std::string answer = object.decode();

    if(answer == primalText)
    std::cout<<"good!   ";

    return 0;
}
