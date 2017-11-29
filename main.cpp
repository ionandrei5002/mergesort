#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <tuple>
#include <queue>

#include "free_list_pool.h"
#include "free_list_allocator.h"

using namespace std;
using mystring = basic_string<char, char_traits<char>, free_list_allocator<char>>;
using pieces = vector<mystring, free_list_allocator<mystring>>;
using strings = vector<pieces, free_list_allocator<pieces>>;

FreeListPool freelist_pool = FreeListPool(200 * 1024 * 1024);
free_list_allocator<uint8_t> free_list_alloca;

// using mystring = string;
// using pieces = vector<mystring>;
// using strings = vector<pieces>;

bool compare(const pieces& first, const pieces& second);
void sortAndWrite(strings& input, ofstream& out, mystring& outString);
void mergeRuns(vector<string>& runs, const string& path);
void split(pieces& results, mystring const& original, char separator);

class MergeData {
public:
    pieces _data;
    istream* _stream;
    bool (*_compFunc)(const pieces& first, const pieces& second);
    MergeData(const pieces& data, istream* stream, bool (*compFunc)(const pieces& first, const pieces& second))
    :_data(data), _stream(stream), _compFunc(compFunc){}
    MergeData& operator=(const MergeData& other) {
        this->_data = other._data;
        this->_stream = other._stream;
        this->_compFunc = other._compFunc;

        return *this;
    }
    bool operator < (const MergeData& toSort) const {
        return !(_compFunc(_data, toSort._data));
    }
};

void split(pieces& results, mystring const& original, char separator)
{
    mystring::const_iterator start = original.begin();
    mystring::const_iterator end = original.end();
    mystring::const_iterator next = find(start, end, separator);
    while (next != end) {
        results.emplace_back(mystring(start, next));
        start = next + 1;
        next = find(start, end, separator);
    }
    results.emplace_back(mystring(start, next));
}

int main()
{
    fstream input("/home/andrei/Desktop/oldies/desktop4/waa_android.csv");
    vector<char, free_list_allocator<char>> rbuffer(24000000);
    input.rdbuf()->pubsetbuf(&rbuffer.front(), rbuffer.size());

    mystring line;
    strings arrayString;

    vector<string> runs;

    int num = 0;
    int part = 0;

    pieces piece;

    mystring outString;

    while(getline(input, line)) {
        split(piece, line, ',');

        arrayString.emplace_back(piece);
        piece.clear();
        num++;

        if (num % 100'000 == 0) {
            string tmpPath = "/home/andrei/Desktop/tmp/";
            tmpPath.append("temp").append(to_string(part)).append(".csv");
            ofstream out(tmpPath);
            vector<char, free_list_allocator<char>> buffer(24000000);
            out.rdbuf()->pubsetbuf(&buffer.front(), buffer.size());

            sortAndWrite(arrayString, out, outString);
            out.close();
            runs.emplace_back(tmpPath);
            part++;
        }

        line.clear();
    }

    if (arrayString.size() > 0) {
        string tmpPath = "/home/andrei/Desktop/tmp/";
        tmpPath.append("temp").append(to_string(part)).append(".csv");
        ofstream out(tmpPath);
        vector<char, free_list_allocator<char>> buffer(24000000);
        out.rdbuf()->pubsetbuf(&buffer.front(), buffer.size());

        sortAndWrite(arrayString, out, outString);
        out.close();
        runs.push_back(tmpPath);
    }

    input.close();

    for(auto it = runs.begin(); it != runs.end(); ++it) {
        cout << *it << endl;
    }

    mergeRuns(runs, "/home/andrei/Desktop/sorted.csv");

    return 0;
}

void mergeRuns(vector<string>& runs, const string& path) {
    vector<fstream> files;

    ofstream out(path);
    vector<char, free_list_allocator<char>> buffer(24000000);
    out.rdbuf()->pubsetbuf(&buffer.front(), buffer.size());

    priority_queue<MergeData> outQueue;

    for(auto it = runs.begin(); it != runs.end(); ++it) {
        string& tmp = *it;
        files.emplace_back(tmp);
    }

    mystring line;
    pieces piece;

    for(auto pairs = files.begin(); pairs != files.end(); ++pairs) {
        fstream& input = *pairs;

        getline(input, line);
        split(piece, line, ',');

        outQueue.emplace(piece, &input, &compare);

        piece.clear();
        line.clear();
    }

    mystring outString;

    int num = 0;

    while(outQueue.empty() == false) {
        MergeData lowest = outQueue.top();

        size_t size = lowest._data.size();

        for(size_t i = 0; i < (size - 1); i++) {
            outString.append(lowest._data[i].data()).append(",");
        }
        outString.append(lowest._data[size - 1].data()).append("\n");
        num++;

        if (num % 100'000 == 0) {
            out << outString;
            outString.clear();
        }

        outQueue.pop();

        if (getline(*(lowest._stream), line)) {
            split(piece, line, ',');

            outQueue.emplace(piece, lowest._stream, &compare);

            piece.clear();
            line.clear();
        }
    }

    if (outString.size() > 0) {
        out << outString;
        outString.clear();
    }

    out.close();
}

void sortAndWrite(strings& input, ofstream& out, mystring& outString) {
    sort(input.begin(), input.end(), &compare);

    auto it = input.begin();
    auto end = input.end();
    for(; it != end; ++it) {
        pieces& toWrite = *it;

        size_t size = toWrite.size();
        if (size > 0) {
            size_t eend = (size - 1);
            for(size_t i = 0; i < eend; i++) {
                outString.append(toWrite[i].data()).append(",");
            }
            outString.append(toWrite[eend].data()).append("\n");
        }
    }
    out << outString;
    outString.clear();

    input.clear();
}

bool compare(const pieces& first, const pieces& second) {
    if (first[10] == second[10]) {
        if (first[11] == second[11]) {
            if (first[12] == second[12]) {
                return false;
            } else {
                return first[12] < second[12];
            }
        } else {
            return first[11] < second[11];
        }
    } else {
        return first[10] < second[10];
    }
}
