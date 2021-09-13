#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
using namespace std;

///global variable and array for all functions
char** pattern=new char* [5];
int CharToInt[256];
char IntToChar[4];

///global variable for Display function
int level=0;
char str[20];

///global flag of create failure link
bool flag=false;

/// global variable to determine each node location in the trie
int numberOfCharAdded=0;

///function to convert from int to char and vice versa
void InitializeTables()
{
    CharToInt['A']=0;
    IntToChar[0]='A';

    CharToInt['C']=1;
    IntToChar[1]='C';

    CharToInt['G']=2;
    IntToChar[2]='G';

    CharToInt['T']=3;
    IntToChar[3]='T';

}

struct Node
{
    Node* Child[4];
    Node* failureLink;
    int ID;
    bool isLeaf;
    int nodeLocation;
};

Node* root;

///function to create node
Node* CreateTrieNode()
{
    Node* node=new Node;
    int i;
    for(i=0; i<4; i++)
    {
        node->Child[i]=0;
        node->failureLink=NULL;
        node->ID=-1;
        node->isLeaf=false;
        node->nodeLocation=0;

    }
    return node;
}


///function to initialize the root of trie
void InitializeTrie()
{
    root=CreateTrieNode();
}

/// function to insert each pattern in the trie
void InsertTriePattern(int* r,int n,int ID,int howManyChar)
{
    Node* cur_Node = root;
    int j;
    int i;
    for(i=0,j=howManyChar; i<n; i++)
    {
        //condition to check if current have a child with the same char
        if(!cur_Node->Child[r[i]])
        {
            cur_Node->Child[r[i]]=CreateTrieNode();
            cur_Node->Child[r[i]]->nodeLocation=j;
            numberOfCharAdded++;
            j++;
        }

        //move node to refer to the child of current node
        cur_Node=cur_Node->Child[r[i]];
    }

    cur_Node->isLeaf=true;
    cur_Node->ID=ID;
}

/// function to conver each pattern to integer array
void InsertRead(char* pattern,int n, int ID,int howManyChar)
{

    // Convert characters to integers
    int r[n];

    for(int i=0; i<n; i++)
        r[i]=CharToInt[(int)pattern[i]];

    // Insert integer array in the trie
    InsertTriePattern(r, n, ID,howManyChar);
}

/// function to print all content of trie
void Display(Node *root,char str[],int level)
{
    Node* curr_Node=root;

    //Condition to print content  of last node of each branch
    if(curr_Node->isLeaf)
    {
        str[level]='\0';
        cout<<str;
        cout<<" ID : "<<curr_Node->ID<<endl;
        cout<<" Node id : "<<curr_Node->nodeLocation<<endl;
        cout<<" failure link : "<<curr_Node->failureLink->nodeLocation<<endl;
    }

    // append on str to print it at the last of loop
    for(int i=0; i<4; i++)
    {
        if(curr_Node->Child[i])
        {
            str[level]= IntToChar[i];
            Display(curr_Node->Child[i],str,level+1);
        }
    }
}

/// function to calculate propersuffix
char* getAllProperSuffixs(int j,string curstring)
{
    int counter=0;
    int length=curstring.length();
    char* propsuffix=new char[20];

    // case of one char there is no propersuffix
    if(length==1)
    {
        propsuffix="";
        return propsuffix;
    }
    else
    {
        //iterate over length of current string and append of propersuffix
        for(int i=j; i<length; i++)
        {
            propsuffix[counter]=curstring[i];
            counter++;
        }
        propsuffix[counter]='\0';
        return propsuffix;
    }
}
/// function to search in trie of a given propersuffix converted to integer numbers
Node* QueryTrieRead(int* r,int n)
{
    int i;
    Node* returnNode;
    Node* cur_node=root;
    for(i=0;i<n;i++)
    {
        // if current node does not have a child with given propersuffix
        if(!cur_node->Child[r[i]])
        {
            returnNode=root;
            return returnNode;
        }

        // if found then failure link will be to the node of given propersuffix
        else
        {
            returnNode=cur_node->Child[r[i]];
            cur_node=cur_node->Child[r[i]];
        }
    }
    return returnNode;
}

/// function to Search in trie of given proper suffixe
Node* SearchInTrie(char* propersuffixs)
{
    // condition to check if length of propersuffix == 0 that is mean there is only one char so failure link will be to the root
    if(strlen(propersuffixs)==0)
        return root;

    int r[strlen(propersuffixs)],i,n=0;

    //iterate over propersuffixs and convert it to integer numbers
    for(i=0; propersuffixs[i]; i++)
    {
        r[i]=CharToInt[propersuffixs[i]];
        n++;
    }

    //Search in the trie depend on integer array
    QueryTrieRead(r,n);
}

/// function to Create failure link of each node in the trie
void CreatFailureLink(Node* node,string curstring)
{
    // flag to use with failure link of each node
    int flag=0;

    //save old current string
    string parent;

    //condition to check if current node is root and if it make failure link equal null
    if(curstring=="")
        node->failureLink=NULL;

    //iterate of 4 char of Sequence (A,C,G,T)
    for(int i=0; i<4; i++)
    {
        parent=curstring;

        //Condition to check if node have a child or not
        if(node->Child[i])
        {
            //Update current string with the current value of pattern
            curstring+=IntToChar[i];

            // if curstring equal 1 char so failureLink of this node will be root
            if(curstring.length()==1)
            {
                char* propersuffixs=getAllProperSuffixs(1,curstring);
                node->Child[i]->failureLink=root;
            }
            else
            {
                //loop iterate over number of proper suffixes of each pattern
                for(int j=1; j<=curstring.length(); j++)
                {
                    // either find the failure link or suffixes has end and does not find any match
                    if(flag!=0)
                      break;

                    //get proper suffixes of current string
                    char* propersuffixs=getAllProperSuffixs(j,curstring);

                    //return failure link of current node depend on function SearchInTrie
                    node->Child[i]->failureLink=SearchInTrie(propersuffixs);

                    // does not match of current proper suffiexes but still there list of proper suffixes
                    if(j<(curstring.length())-1 && node->Child[i]->failureLink==root)
                      continue;

                    // either find the failure link of suffixes had end and does not find any match
                    else
                    {
                      flag=1;
                      node->Child[i]->failureLink=SearchInTrie(propersuffixs);
                    }

                }
                 flag=0;
            }
            CreatFailureLink(node->Child[i],curstring);
            curstring=parent;
        }

    }

}
/// function to Search of number of pattern in given text
void Search(Node* root,char* Text)
{
    int i=0;
    Node* j=root;

    // print first location at root and start of text
    cout<< "( t "<<i<<","<<" p "<<j->nodeLocation<<" ) "<<endl;

    while(i<=strlen(Text))
    {
        // Condition to check if current node have a child with the same char of text or not
        if(j->Child[CharToInt[Text[i]]] != NULL  )
        {
            // print location of i and location of pattern
            cout<< "( t "<<i+1<<","<<" p "<<j->Child[CharToInt[Text[i]]]->nodeLocation<<" ) ";

            // condition to check after matching has been done if failure link of current node have ID or not
            if(j->Child[CharToInt[Text[i]]]->failureLink->ID !=-1)
                cout<<" rep ( "<<j->Child[CharToInt[Text[i]]]->failureLink->ID<<" , "<<(i-strlen(pattern[j->Child[CharToInt[Text[i]]]->failureLink->ID]))+1<<" )  ";

            // condition to check if current node have ID or not
            if (j->Child[CharToInt[Text[i]]]->ID != -1)
                cout<< " rep ( "<<j->Child[CharToInt[Text[i]]]->ID<< " , "<<(i-strlen(pattern[j->Child[CharToInt[Text[i]]]->ID]))+1<<" )  ";

            j=j->Child[CharToInt[Text[i]]];
            i++;

            cout<<endl;
        }

        else
        {
            // Condition to check if current node is root that is mean no failure link so we have to move i counter of text
            if(j->nodeLocation == 0)
                i++;
            // There is a failure link so move counter of pattern to failure link
            else
                j=j->failureLink;
            cout<< "( t "<<i<<","<<" p "<<j->nodeLocation<< " ) "<<endl;
        }

        //end of matching if i has reach to the end to text and current node is root
        if(i==strlen(Text) && j->nodeLocation==0 )
            break;
    }

}

///function to delete the trie
void DeleteTrie(Node* node)
{
    int i;
    for(i=0; i<4; i++) if(node->Child[i]) DeleteTrie(node->Child[i]);
    delete node;
}
int main()
{
    InitializeTables();
    InitializeTrie();

    pattern[0]="AGA";
    pattern[1]="AA";
    pattern[2]="AAG";
    pattern[3]="GAAG";
    pattern[4]="TCG";

    char* Text="GAACAAGTGAAGTGAGAAGAAGT";

    int howManyChar=1;

    // loop over array of pattern and insert each pattern on the trie
    for(int i=0; i<5; i++)
    {
        InsertRead(pattern[i],strlen(pattern[i]),i,howManyChar);

        // counter to count how many char have been added to the trie so we and determine each node location in trie
        howManyChar=numberOfCharAdded+1;
    }
    CreatFailureLink(root,"");
    Search(root,Text);
   //Display(root,str,level);
   DeleteTrie(root);

    return 0;
}
