/**
 *    Implementation of Inverted Index for 181 documents.
 * -----------------------------------------------------------
 * Every document is parsed into terms and index is made on the terms such that retrieval becomes efficient.
*/

//Required header files
#include<bits/stdc++.h>
#include<fstream>
#include<sstream>
#include<dirent.h>
using namespace std;

#define N 1000
#define BASEPATH "C:\\Users\\Rukmini Meda\\Desktop\\BTech\\Academics\\Sem_5\\IR\\Project\\102-Project-IR-MERUK\\code\\dataset\\"

void parseQuery(string query, vector<string>& queryTerms){
    int length = query.size();
    int start = 0;
    int i=0;
    for(;i<length;i++){
        if(query[i] == ' '){
            string temp = query.substr(start,i-start);
            if(temp != " "){
                queryTerms.push_back(temp);
            }
            start = i+1;
        }
    }
    string temp = query.substr(start,i-start);
    if(temp != " "){
        queryTerms.push_back(temp);
    }
    start = i+1;
}

vector<string> parseDocs(int docid){
    string filename = BASEPATH+to_string(docid)+".txt";
    fstream file;
    file.open(filename,ios::in);
    vector<string> res;
    if(!file){
        cout << "Document not found to parse!!" << endl;  
        return res;   //Warning
    }
    else{
        string str;
        int wordCount = 0;
        while(getline(file,str)){
            int len = str.size();
            string word = "";
            for(int i=0;i<len;i++){
                if(str[i] == ' '||str[i]=='\t'||str[i]=='\n'|| ispunct(str[i]) || str[i]<0 || str[i]>128){
                    if(word != ""){
                        wordCount++;
                        res.push_back(word);
                        word = "";
                    }
                }
                else{
                    word+=str[i];
                }
            }
        }
    }
    file.close();
    return res;
}

double dotProduct(vector<double> q,vector<double> d){
    double res = 0;
    int s = q.size();
    for(int i=0;i<s;i++){
        res += (q[i]*d[i]);
    }
    return res;
}

double norm(vector<double> v){
    double res;
    int s = v.size();
    for(int i=0;i<s;i++){
        res += (v[i]*v[i]);
    }
    return sqrt(res);
}

void calculateCosineSimilarity(vector< pair<double,int> >& simscores,vector<double> queryVector,vector< pair<int, vector<double> > > docVectors){
    int docsize = docVectors.size();
    for(int i=0;i<docsize;i++){
        double docValue = dotProduct(queryVector,docVectors[i].second);
        double norm1 = norm(queryVector);
        double norm2 = norm(docVectors[i].second);
        // cout << docValue << " " << norm1 << " " << norm2 << endl;
        // cout << "    " << (double)docValue/(norm1*norm2) << "     " << endl;
        simscores.push_back(make_pair(docValue/(norm1 * norm2),docVectors[i].first));
    }
}

// Class definition for a node in a bst
class Node{
public:
    //Document ID, frequency in that doc
    int doc_num;
    int freq;
    vector<int> positions;
    Node *left,*right;
    //Constructor
    Node(int doc,int index){
        this->doc_num = doc;
        this->positions.push_back(index);
        this->freq = 1;
        this->left = NULL;
        this->right = NULL;
    }
};

//Class definition for a BST
class BST{
public:
    //Root of the BST
    Node *root;
    //Constructor
    BST(){
        this->root = NULL;
    }
    //Utility function to search for a document in the BST
    Node* search(Node *root,int id){
        if(root != NULL){
            if(root->doc_num == id){
                return root;    //Document found
            }
            else if(root->doc_num<id){
                Node* val1 = search(root->right,id);
                if(val1 != NULL){
                    return val1;
                }
            }
            else{
                Node* val1 = search(root->left,id);
                if(val1 != NULL){
                    return val1;
                }
            }
        }
        return NULL;    //Document not found
    }

    //Utility function to insert a new document in the BST
    Node* insert_doc(Node *root,int id,int index){
        if(root == NULL){
            return new Node(id,index);     //Creating a new node and returning for insertion
        }
        if(root->doc_num<id){
            root->right = insert_doc(root->right,id,index);
        }
        else{
            root->left = insert_doc(root->left,id,index);
        }
        return root;
    }

    //Utility function to update a new document in the BST.
    void update_doc(Node *root,int id,int index){
        if(root->doc_num == id){
            root->freq++;
            root->positions.push_back(index);        //Frequency of that doc incremented.
            return;
        }
        else if(root->doc_num<id){
            update_doc(root->right,id,index);
        }
        else{
            update_doc(root->left,id,index);
        }  
    }

    //Utility function for inorder traversal of the BST
    void inorderTraversal(Node *node,string& str2){
        if(node != NULL){
            if(str2 != ""){
                str2 += "; ";
            }
            str2 += to_string(node->doc_num);
            str2 += "=";
            str2 += to_string(node->freq);
            str2 += ",[";
            int length = node->positions.size();
            for(int i=0;i<length;i++){
                str2 += to_string(node->positions[i]);
                if(i != length - 1){
                    str2 += ", ";
                }
            }
            str2 += "]";
            inorderTraversal(node->left,str2);
            inorderTraversal(node->right,str2);
        }
    }
};

class InvertedIndex{
    public:
    //Inverted index structure
    map< string,pair<int,BST*> > dictionary;

    //Utility function to update index for any term found.
    void update_index(string word,int doc,int index){
        bool flag = true;
        if(this->dictionary.find(word)==this->dictionary.end()){
            this->dictionary[word].first = 1;
            this->dictionary[word].second = new BST();
            flag = false;
        }
        BST *tree = (this->dictionary[word].second);
        bool val = (*tree).search(tree->root,doc) != NULL;
        if(val == true){
            (*tree).update_doc(tree->root,doc,index);    //Updating doc
        }
        else{
            tree->root = (*tree).insert_doc(tree->root,doc,index);  //Inserting doc
            if(flag == true){
                this->dictionary[word].first++;
            }
        }
    }
    //Utility function to write the index to file.
    void writeIndex(map<string,int>& lines){
        map<string, pair<int,BST*> >::iterator it;
        fstream file,file1;
        string str ="";
        int i=0;
        file.open("output.txt",ios::out);                   //Opening the file
        for(it=this->dictionary.begin();it!=this->dictionary.end();it++){
            file << it->first << " (" << (it->second).first << ") ==> {";
            string str="";
            BST *tree = (it->second).second;
            tree->inorderTraversal(tree->root,str);
            file << str << "}" << endl;
            lines[it->first] = i;
            i++;
        }
        file.close();                     //Closing the file
    }

    vector<double> getTFVector(vector<string>& queryVector, vector<string>& docVector){
        map<string,double> count;
        int docTerms = docVector.size();
        for(int i=0;i<docTerms;i++){
            if(count.find(docVector[i]) == count.end()){
                count[docVector[i]] = 1;
            }
            else{
                count[docVector[i]]++;
            }
        }
        // map<string,double>::iterator it;
        // for(it=count.begin();it!=count.end();it++){
        //     cout << it->first << " -> " << it->second << endl;
        // }
        int queryTerms = queryVector.size();
        vector<double> res;
        for(int i=0;i<queryTerms;i++){
            double tf = count[queryVector[i]];
            double idf = log((double)N/(this->dictionary[queryVector[i]].first));
            // cout << "tf is " << tf << endl;
            // cout << "idf is " << idf << endl;
            // cout << "res is " << (1+log(tf))*idf << endl;
            res.push_back((1+log(tf))*idf);
        }
        return res;
    }

    bool cmp(pair<double,int> p1, pair<double,int> p2){
        return p1.first > p2.first;
    }
    
    vector<int> rankDocs(vector<int>& doclist,string query){
        // cout << "Entered" << endl;
        vector<string> queryTerms;
        parseQuery(query,queryTerms);
        // cout << "parsed query" << endl;
        int numTermsInQuery = queryTerms.size();
        vector<double> queryVector = this->getTFVector(queryTerms,queryTerms);
        // cout << "query tf obtained" << endl;
        vector< pair<int, vector<double> > > docVectors;
        int numDocs = doclist.size();
        for(int i=0;i<numDocs;i++){
            vector<string> docterms = parseDocs(doclist[i]);
            // cout << "parsed doc" << endl;
            docVectors.push_back(make_pair(doclist[i],this->getTFVector(queryTerms,docterms)));
        }
        // cout << "looped" << endl;
        vector< pair<double,int> > simscores;
        calculateCosineSimilarity(simscores,queryVector,docVectors);
        // cout << "cosine done " << endl;
        sort(simscores.begin(),simscores.end());
        vector<int> rankedlist;
        int l = simscores.size();
        for(int i=l-1;i>=0;i--){
            rankedlist.push_back(simscores[i].second);
        }
        // cout << "ranked" << endl;
        return rankedlist;
    }

    void freeTextQuery(map<string,int>& linenumbers,string query){
        // string query;
        // getline(cin,query);
        vector<string> queryTerms;
        parseQuery(query,queryTerms);
        int s1 = queryTerms.size();
        // for(int i=0;i<s1;i++){
        //     cout << queryTerms[i] << endl;
        // }
        set<int> docsSet;
        vector<int> tempV;
        for(int i=0;i<s1;i++){
            tempV = this->oneWordQuery(linenumbers,queryTerms[i]);
            vector<int>::iterator it;
            for(it=tempV.begin();it!=tempV.end();it++){
                // cout << *it << endl;
                docsSet.insert(*it);
            }
        }
        cout << " ----- " << endl;
        fstream file;
        set<int>::iterator its;
        vector<int> doclist;
        for(its=docsSet.begin();its!=docsSet.end();its++){
            // cout << *its << " " << endl; 
            doclist.push_back(*its);
            string filename = BASEPATH + to_string(*its) + ".txt";
            // file.open(filename,ios::in);
            // string str;
            // while(getline(file,str)){
            //     cout << str << endl;
            // }
            // file.close();
        }
        vector<int> ranklist;
            ranklist = this->rankDocs(doclist,query);
            cout << ranklist.size() << endl;
            int l1 = ranklist.size();
            vector<int>::iterator it;
            for(it=ranklist.begin();it!=ranklist.end();it++){
                cout << (*it) << endl;
            }
            // fstream file;
            for(int i=0;i<l1;i++){
                cout << ranklist[i] <<" "<< "is docid" << endl;
                string str;
                file.open(BASEPATH+to_string(ranklist[i])+".txt",ios::in);
                while(getline(file,str)){
                    cout << str << endl;
                }
                file.close();
            }
            // return ranklist;
    }


    vector<int> oneWordQuery(map<string,int>& linenumbers,string query){
        // string query;
        // cin >> query;
        vector<int> temp;
        if(linenumbers.find(query) == linenumbers.end()){
            cout << "No documents found!" << endl;
            return temp;
        }
        else{
            fstream file;
            string indexfilename = "output.txt";
            file.open(indexfilename,ios::in);
            if(!file){
                cout << "File not found!!" << endl;
                return temp;
            }
            int no = linenumbers[query];
            file.seekg(ios::beg);
            for(int i=0;i<no;i++){
                file.ignore(numeric_limits<streamsize>::max(),'\n');
            }
            string str;
            getline(file,str);
            file.close();
            cout << str << endl;
            // map<string, pair<int, map<int, vector<int> > > > res;
            // parseIndexOfWord(res,str);
            // return temp;
            int length = str.size();
            int start, end;
            for(int i=0;i<length;i++){
                if(str[i] == '('){
                    start = i;
                }
                else if(str[i] == ')'){
                    end = i;
                    break;
                }
            }
            // cout << start << " " << end << endl;
            string temp = str.substr(start+1,end-start-1);
            // cout << temp << endl;
            int df = stoi(temp);
            // cout << df << endl;
            vector<int> doclist;
            for(int i=0;i<length;i++){
                if(str[i] == '{' || (str[i] == ' ' && str[i-1] == ';')){
                    // cout << i << endl;
                    start = i;
                }
                else if(str[i] == '=' && (isdigit(str[i+1]) == true)){
                    // cout << i << endl;
                    end = i;
                    // cout << start << " " << end << endl;
                    string temp = str.substr(start+1,end-start-1);
                    int docid = stoi(temp);
                    // cout << docid << endl;
                    doclist.push_back(docid);
                    start = i + 1;
                }
            }
            int l1 = doclist.size();
            vector<int> ranklist;
            ranklist = this->rankDocs(doclist,query);
            cout << ranklist.size() << endl;
            vector<int>::iterator it;
            for(it=ranklist.begin();it!=ranklist.end();it++){
                cout << (*it) << endl;
            }
            // fstream file;
            for(int i=0;i<l1;i++){
                cout << ranklist[i] <<" "<< "is docid" << endl;
                file.open(BASEPATH+to_string(ranklist[i])+".txt",ios::in);
                string str;
                while(getline(file,str)){
                    cout << str << endl;
                }
                file.close();
            }
            return ranklist;
        }
    }

};

//Driver function of the program
int main(){
    // cout << "Entered" << endl;
    clock_t start, end;
    start = clock();
    //Opening the directory for the dataset
    // DIR *dir;
    // dirent *pdir;
    // string files[200];
    // dir = opendir("C:\\Users\\Rukmini Meda\\Desktop\\BTech\\Academics\\Sem_5\\IR\\Project\\dataset");
    // int i=0;
    // string path = "C:\\Users\\Rukmini Meda\\Desktop\\BTech\\Academics\\Sem_5\\IR\\Project\\dataset\\";
    // while((pdir=readdir(dir))!=NULL){
    //     files[i] = "C:\\Users\\Rukmini Meda\\Desktop\\BTech\\Academics\\Sem_5\\IR\\Project\\dataset\\";
    //     files[i] += pdir->d_name;
    //     cout << files[i] << endl;
    //     i++;
    // }
    // cout << path << endl;
    // closedir(dir);
    InvertedIndex index;
    //Looping over all documents to create inverted index
    for(int i=0;i<N;i++){
        // cout << i << endl;
        fstream file;
        int doc_number = i;
        string filename = BASEPATH+to_string(i)+".txt";
        cout << filename << endl;  //Generalizing the doc ID.
        file.open(filename,ios::in);
        if(!file){
            cout << "File not found!!" << endl;     //Warning
        }
        else{
            string str;
            int line = 1;
            int wordCount = 0;
            while(getline(file,str)){
                int len = str.size();
                string word = "";
                for(int i=0;i<len;i++){
                    if(str[i] == ' '||str[i]=='\t'||str[i]=='\n'|| ispunct(str[i]) || str[i]<0 || str[i]>128){
                        if(word != ""){
                            wordCount++;
                            index.update_index(word,doc_number,wordCount);    //Updating the index for the term found
                            word = "";
                        }
                    }
                    else{
                        word+=str[i];
                    }
                }
            }
        }
        file.close();
    }
    map<string,int> linenumbers;
    index.writeIndex(linenumbers);
    // map<string,int>::iterator it;
    // for(it=linenumbers.begin();it!=linenumbers.end();it++){
    //     cout << it->first << " " << it->second << endl;
    // }     //Outputting the index to file
    end = clock();
    double time_taken = double(end - start) / double(CLOCKS_PER_SEC); 
    cout << "Time taken by program is : " << fixed  
         << time_taken << setprecision(5); 
    cout << " sec " << endl; 
    cout << "Total number of terms are " << index.dictionary.size() << endl;

    string query;
    getline(cin,query);

    int choice = 0;
    
    switch(choice){
        case 0: index.oneWordQuery(linenumbers,query);break;
        case 1: index.freeTextQuery(linenumbers,query);break;
        // case 2: phraseQuery(linenumbers,path,query);break;
        default: break;
    }
    return 0;
}