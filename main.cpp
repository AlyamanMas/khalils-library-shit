#include <iostream>
#include <cstring>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

void bruteForceSearch(string text, string pattern)
{
    int n = text.size();
    int m = pattern.size();

    for (int i = 0; i <= n - m; i++) {
        int j;
        for (j = 0; j < m; j++) {
            if (text[i + j] != pattern[j])
                break;
        }
        if (j == m) {
            cout << "Pattern found at index " << i << endl;
        }
    }
}


vector<int> computeLPS(string pattern) {
    int m = pattern.length();
    vector<int> lps(m, 0);
    int len = 0;
    int i = 1;
    while (i < m) {
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = len;
            i++;
        }
        else {
            if (len != 0) {
                len = lps[len - 1];
            }
            else {
                lps[i] = 0;
                i++;
            }
        }
    }
    return lps;
}

void KMP(string pattern, string text) {
    int n = text.length();
    int m = pattern.length();
    vector<int> lps = computeLPS(pattern);
    int i = 0, j = 0;
    while (i < n) {
        if (text[i] == pattern[j]) {
            i++;
            j++;
        }
        if (j == m) {
            cout << "Pattern found at index " << i - j << endl;
            j = lps[j - 1];
        }
        else if (i < n && text[i] != pattern[j]) {
            if (j != 0) {
                j = lps[j - 1];
            }
            else {
                i++;
            }
        }
    }
}





void rabinKarpSearch(string text, string pattern)
{
    int d = 256; // number of possible characters in the input alphabet
    int q = 101; // a prime number

    int n = text.size();
    int m = pattern.size();

    int i, j;
    int p = 0; // hash value for pattern
    int t = 0; // hash value for text
    int h = 1;

    // calculate h = d^(m-1)
    for (i = 0; i < m - 1; i++) {
        h = (h * d) % q;
    }

    // calculate hash value of pattern and first window of text
    for (i = 0; i < m; i++) {
        p = (d * p + pattern[i]) % q;
        t = (d * t + text[i]) % q;
    }

    // slide the pattern over text one by one
    for (i = 0; i <= n - m; i++) {

        // check if hash values of current window of text and pattern match
        if (p == t) {
            // check for characters one by one
            for (j = 0; j < m; j++) {
                if (text[i + j] != pattern[j])
                    break;
            }
            if (j == m) {
                cout << "Pattern found at index " << i << endl;
            }
        }

        // calculate hash value for next window of text
        if (i < n - m) {
            t = (d * (t - text[i] * h) + text[i + m]) % q;
            if (t < 0) {
                t += q;
            }
        }
    }
}
#include <fstream>
#include <random>
#include <ctime>
#include <algorithm>
#include <map>
#include <sstream>

void readWordsFromFile(string filePath, vector<string>& words) {
    ifstream file(filePath);
    string line;
    while(getline(file, line)) {
        words.push_back(line);
    }
    file.close();
}

void readTextFromFile(string filePath, string& text) {
    ifstream file(filePath);
    stringstream buffer;
    buffer << file.rdbuf();
    text = buffer.str();
    file.close();
}

void benchmark(string text, vector<string> words, int numTrials) {
    map<string, vector<double>> timings;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, words.size() - 1);

    for (int i = 0; i < numTrials; i++) {
        string pattern = words[dis(gen)];

        auto start = high_resolution_clock::now();
        bruteForceSearch(text, pattern);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        timings["Brute Force"].push_back(duration.count());

        start = high_resolution_clock::now();
        KMP(pattern, text);
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start);
        timings["KMP"].push_back(duration.count());

        start = high_resolution_clock::now();
        rabinKarpSearch(text, pattern);
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start);
        timings["Rabin Karp"].push_back(duration.count());
    }

    ofstream file("benchmark_results.csv");
    file << "Brute Force,KMP,Rabin Karp\n"; // header
    for (int i = 0; i < numTrials; i++) {
        file << timings["Brute Force"][i] << ",";
        file << timings["KMP"][i] << ",";
        file << timings["Rabin Karp"][i] << "\n";
    }
    file.close();
}

int main() {
    vector<string> words;
    string text;
    readWordsFromFile("words.txt", words);
    readTextFromFile("text.txt", text);
    benchmark(text, words, 100); // replace 100 with the number of trials you want
}