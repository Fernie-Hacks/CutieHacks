#include <stdio.h>
#include <curl/curl.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <exception>

using std::ostream;
using std::ostringstream;
using std::streamsize;
using std::string;
using std::cout;
using std::endl;

static string main_url = "http://genius.com/";

static size_t data_write(void* buffer, size_t size, size_t nmemb, void* userp)
{
    if(userp) {
        ostream &os = *static_cast<ostream *>(userp);
        streamsize len = size *nmemb;
        if(os.write(static_cast<char *>(buffer), len))
            return len;
    }
    return 0;
}

CURLcode curl_read(const string &url, ostream &os, long timeout = 30)
{
    CURLcode code(CURLE_FAILED_INIT);
    CURL *curl = curl_easy_init();
    
    if(curl) {
        if(CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_write))
           && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L))
           && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L))
           && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FILE, &os))
           && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout))
           && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str())))
        {
            code = curl_easy_perform(curl);
        }
        curl_easy_cleanup(curl);
    }
    return code;
}

int main(int argc, char*argv[])
{
    if(argc != 2) {
        cout << "Usage: ./main /directory/to/song.mp3 "<< endl;
        return 1;
    }
    
    string url_ext(argv[1]);
    if(url_ext.size() <= 0) {
        cout << "path is empty" << endl;
        return 1;
    }
    std::ofstream out;
    std::size_t found = url_ext.find_last_of("/\\");
    string song = url_ext.substr(found + 1);
    url_ext = url_ext.substr(0, found);
    string filepath = url_ext;
    filepath.append("/lyrics.txt");
    cout << filepath << endl;
    
    //get rid of album
    found = url_ext.find_last_of("/\\");
    url_ext = url_ext.substr(0, found);
    
    //get artist name
    found = url_ext.find_last_of("/\\");
    string artist = url_ext.substr(found + 1);
    url_ext = url_ext.substr(0, found);
    url_ext.clear();
    found = artist.find(' ');
    //cout << "Artist:" << artist << endl;
    while(found != string::npos) {
        url_ext.append(artist.substr(0, found));
        url_ext.append("-");
        artist = artist.substr(found + 1);
        found = artist.find(' ');
    }
    url_ext.append(artist);
    url_ext.append("-");
    found = song.find(' ');
    while(found != string::npos) {
        string possilbe_int = song.substr(0, found);
        try {
            int x = std::stoi(possilbe_int, NULL, 10);
            song = song.substr(found + 1);
            found = song.find(' ');
            continue;
        } catch(std::exception &e) {
            url_ext.append(possilbe_int);
        }
        url_ext.append("-");
        song = song.substr(found + 1);
        found = song.find(' ');
    }
    found = song.find(".mp3");
    song = song.substr(0, found);
    url_ext.append(song);
    url_ext.append("-lyrics");
    
    CURL *curl;
    CURLcode res;
    ostringstream oss;
    string url;
    //adding "http://genius.com/"
    url.append(main_url);
    url.append(url_ext);
    
    curl_global_init(CURL_GLOBAL_ALL);
    if(CURLE_OK == curl_read(url, oss)) {
        // Web page successfully written to string
        string html = oss.str();
        std::size_t index = html.find("<p>");
        if(index != string::npos) {
            html = html.substr(index + 3);
            index = html.find("</p>");
            html = html.substr(0, index);
            
            index = html.find("<a");
            while(index != string::npos) {
                std::size_t end = html.find(">", index);
                html = html.replace(index, end - index + 1, "");
                index = html.find("</a>");
                html = html.replace(index, 4, "");
                index = html.find("<a");
            }
        }
        else {
            index = html.find("Verse 1");
            if(index != string::npos) {
                html = html.substr(index);
                index = html.find("</p>");
                html = html.substr(0, index);
                
                index = html.find("<a");
                while(index != string::npos) {
                    std::size_t end = html.find(">", index);
                    html = html.replace(index, end - index + 1, "");
                    index = html.find("</a>");
                    html = html.replace(index, 4, "");    			
                    index = html.find("<a");
                }
            }
        }
        found = html.find("<br>");
        while(found != string::npos) {
            html.replace(found, 4, "");
            found = html.find("<br>", found + 1);
        }
        out.open("lyrics.txt");
        if(out.is_open()) {
            out << html;
        }
        out.close();
    }
    else {
        cout << "Error occurred getting page html" << endl;
    }
    return 0;
}