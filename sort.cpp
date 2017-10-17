#include <vector>
#include <algorithm> // This is where you find std::sort
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <sstream>

using namespace std;

/*
  mergeFiles takes 2 sorted input files and created a thrid file
  that will have the merged sorted data.
  string inFile1 - input
  string inFile2 - input
  string outFile - input
  Does not return anything back to the caller
*/
void mergeFiles(string inFile1, string inFile2, string outFile) {
    // Open Files
    ifstream fin(inFile1);
    ifstream fin2(inFile2);
    ofstream fout(outFile);

    string line1;               // string to hold line from file1
    string line2;               // string to hold line from file2
    int i = 1;                  // line counter
    
    if(fin) {                   // if file is open
        getline(fin,line1);      // get first line
    }
    if(fin2) {                  // if file is open
        getline(fin2,line2);     // get first line
    }
    while(fin || fin2) {        // if either file is still open
        if(fin && fin2) {       // if both files are still open
            if((line1.compare(line2)) > 0) {    // file 2 has the smaller 
                fout << line2 << endl;          // print line2
                getline(fin2, line2);           // get next line from file 2
            }
            else {                      // file 1 has smaller
                fout << line1 << endl;  // print line1
                getline(fin, line1);    // get next line from file 1
            }
        }//endif
        else {      // if one of the files has finished
            if(fin){
                fout << line1 << endl;
                getline(fin, line1); 
            }         
            else if(fin2){
                fout << line2 << endl;
                getline(fin2, line2);
            }   
        }// endelse
    }//endwhile
}//endmethod


/*
  mergefiles_pass: Is a recursive function that used for each pass
  string input_prefix - input
  int fileCount - input
  int newFileCount - input
  returns back the number of intermediatery temp files that were created for the pass

  NOTE: All intermediate temp files are automatically deleted.
*/
int mergefiles_pass(string input_prefix, int fileCount, int newFileCount){

    string file_a=input_prefix;
    string intBuf;
    stringstream ss_a;
    ss_a << fileCount;
    intBuf = ss_a.str();
    file_a.append(intBuf).append(".tmp");

    string file_b=input_prefix;
    string intBuf_b;
    stringstream ss_b;
    ss_b << fileCount-1;
    intBuf_b = ss_b.str();
    file_b.append(intBuf_b).append(".tmp");

    string output_file="";
    output_file = input_prefix+"pass_";
    string intBuf_o;
    stringstream ss_o;
    ss_o << newFileCount;
    intBuf_o = ss_o.str();
    output_file.append(intBuf_o).append(".tmp");

    if (fileCount == 1){
        //We have reached the end of the merge
        cout<< "End of the all merge for prefix: [" <<input_prefix<<"?.tmp]"<<endl;
        std::ifstream infile(file_a.c_str());
        if (infile.good()){
            rename(file_a.c_str(), "final_sorted.dat");
            cout<<" Intermediatery merge file complete..." <<endl;
        } else{
            cout<<" Error: Something went wrong..."<<endl;
        }
        return newFileCount-1; 
    }

    mergeFiles(file_a, file_b, output_file);   
    std::remove(file_a.c_str()); 
    std::remove(file_b.c_str()); 
    if (fileCount<=2){
        //REcursive call Exit
        return newFileCount;
    } else{
         // REcursive call 
        return mergefiles_pass(input_prefix, fileCount-2, newFileCount+1);
    }
}

/*
  Helper function to split the given string into words using given delimiter
*/
void split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

/* The main function that calls all the recursive function 
    input - int - memorySize - Size of memory available for use.
    input - string - input txt file to be sorted.

    The basic logic is the input file is chunked into N smaller files
    where size of each chumk is not more than memorySize.
    - Each file is then loaded into memory to do in-memory sort.
    - And all the sorted chuked files are merged back into one file again.

    Caveat: If a single word/string is bigger than given memorySize then
    we are not taking care of that. 

    The input text file is parsed line by line and the each string/word is 
    extracted. the word delimiter is space, tab, or new line.

*/
int sort_file(int memorySize, string inputFile)
{       
    int memSize = memorySize;

	// Open your text file
	std::ifstream file(inputFile);
	std::vector<std::string> rows;
    
    int filledBuffer=0;
    int fileCount = 1;
	// Read all the lines and add them to the rows vector
	while(!file.eof())
	{
		std::string line;
        std::getline(file, line);
        
        vector<string> aListofWords;
        stringstream sstoext(line);
        string sWordBuf;

        while (sstoext >> sWordBuf){

            int wordSize = sWordBuf.size();
            if (filledBuffer+wordSize > memSize){

                //1. Sort the buffered words.
                //2. Save the filledBuffer to file
                //3. And reset the filledBuffer

                string fullChunkName;
                stringstream ss;
                ss << fileCount;
                string intBuf = ss.str();
                fullChunkName.append(intBuf).append(".tmp");  

                ofstream outputfile;
                outputfile.open (fullChunkName);

                std::sort(rows.begin(), rows.end());
                // Save all of the vectors values to the tmp file.
                std::vector<std::string>::iterator iterator = rows.begin();
                for(; iterator != rows.end(); ++iterator)
                {
                    outputfile << *iterator+"\n";
                }
                outputfile.close();

                rows.clear();
                fileCount= fileCount+1;
                rows.push_back(sWordBuf);     
                filledBuffer =0;           
            }
            else{
                rows.push_back(sWordBuf);
                filledBuffer = filledBuffer + wordSize;
            }
        }
    }
    
    if(!rows.empty()){

        //Save the last left out data
        string fullChunkName;
        stringstream ss;
        ss << fileCount;
        string intBuf = ss.str();
        fullChunkName.append(intBuf).append(".tmp");  

        //////
        ofstream outputfile;
        outputfile.open (fullChunkName);

        std::sort(rows.begin(), rows.end());
   
        // Save all of the vectors values to the tmp file.
        std::vector<std::string>::iterator iterator = rows.begin();
        for(; iterator != rows.end(); ++iterator)
        {
            outputfile << *iterator+"\n";
        }
        outputfile.close();
        rows.clear();
        fileCount= fileCount+1;        
    }

    cout<< endl<<"Pass1: Total temp files (.tmp) created = " << fileCount-1 <<endl;

    int tot = fileCount-1;
    string pass="";
    while (tot>=1){

        tot = mergefiles_pass(pass,tot,1);
        pass = pass+"pass_";
    }  


    std::ifstream infile("final_sorted.dat");
    if (infile.good()){
        cout<<endl;
        cout<<"Success: Sorting complete and sorted file is =======> final_sorted.dat" <<endl;
        cout<<"Note all temp files that were created are deleted automatically." <<endl;
        cout<<"------"<<endl;
        return 0; // success
    } else{
        cout<<endl;
        cout<<" Error: Something went wrong..."<<endl;
        cout<<"------"<<endl;
        return -1; //failure
    }


    return 0;
}

int main(){

    cout<<"----- External Sort [Start] ------"<<endl<<endl;
    try{
        int memSize;
        string fileName;
        cout<< "Enter the available memory in bytes: ";
        cin >> memSize;
        cout<< "Enter the file name (it has to be in the current directory): ";
        cin >> fileName;
        return sort_file(memSize, fileName);
    }catch(std::exception const& e){
        std::cerr << e.what() << std::endl;
        return -1;
    }
}