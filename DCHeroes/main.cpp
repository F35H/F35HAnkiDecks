#include <Urlmon.h>
#pragma comment(lib,"Urlmon.lib")

#include <fstream>
#include <array>
#include <string>
#include <queue>
#include <deque>
#include <iostream>



int main() {
  std::deque<std::wstring> url_databases = {};
  std::queue<std::wstring> url_q = {};
  std::wstring file_content;
  std::wstring file = L"html_file.html";
  uint8_t find_link = 0;


  url_databases.push_back(L"https://dc.fandom.com/wiki/Category:Good_Characters");
  url_databases.push_back(L"https://dc.fandom.com/wiki/Category:Bad_Characters");
  url_databases.push_back(L"https://dc.fandom.com/wiki/Category:Teams");

  static std::wofstream o_stream = std::wofstream("dc_heroes.csv");
  static std::wifstream stream;
  for (; !url_databases.empty(); url_databases.pop_front()) {
    stream.close();
    auto val = URLDownloadToFile(NULL, url_databases.front().c_str(), file.c_str(), 0, NULL);
    if (val == 0x800c000d) continue;
    stream = std::wifstream(file);
    RESET_LOOP:
    while (std::getline(stream, file_content)) {
      if (!(file_content.find(L"class=\"category-page__member-link\"") == std::string::npos)) {
        auto quote_pos = file_content.find('"');
        auto second_quote = file_content.find(L"class");
        auto url_str = file_content.substr(quote_pos + 1, second_quote - quote_pos - 3);
        url_q.push(L"https://dc.fandom.com" + url_str);
      }; //

      if (!(file_content.find(L"class=\"category-page__pagination-prev") == std::string::npos)) { url_databases.pop_back(); ++find_link; };
      if (!(file_content.find(L"class=\"category-page__pagination\"") == std::string::npos))++find_link;
      if (find_link && !(file_content.find(L"href=") == std::string::npos)) {
        if (file_content.find(L"xlink:href=") != std::string::npos) goto RESET_LOOP;
        auto quote_pos = file_content.find(L"href=");
        auto second_quote = file_content.length();
        auto url_str = file_content.substr(quote_pos + 6, second_quote-quote_pos-7);
        url_databases.push_back(url_str);
        find_link = 0;
      }; 
    }; //for every line in a file
  }; //empty

  std::wstring content;
  std::wstring thumb;
  uint32_t content_switch = 0;
  uint64_t q_pos = 0;
  for (; !url_q.empty(); url_q.pop()) {
    stream.close();
    auto val = URLDownloadToFile(NULL, url_q.front().c_str(), file.c_str(), 0, NULL);
    if (val == 0x800c000d) continue;
    stream = std::wifstream(file);

    LOOP_OVER:
    while (std::getline(stream, file_content)) {
      switch (content_switch) {
      default:
        q_pos = file_content.find(L"<aside");
        if (q_pos == std::string::npos) goto LOOP_OVER;
        content += file_content.substr(q_pos, file_content.size() - q_pos);
        content_switch++;
        goto LOOP_OVER;
      case 1: {
        content += file_content;
        q_pos = file_content.find(L"/aside");
        if (q_pos != std::string::npos) content_switch++;
        
        q_pos = file_content.find(L"<img");
        if (q_pos != std::string::npos) 
          thumb = file_content;

        goto LOOP_OVER;
      }; //content_switch
      case 2: {
        q_pos = file_content.find(L"<p>");
        if (q_pos == std::string::npos) goto LOOP_OVER;
        content += file_content.substr(q_pos, file_content.size() - q_pos);
        content_switch++;
        goto LOOP_OVER;
      }; //content_switch
      case 3: {
        content += file_content;
        q_pos = file_content.find(L"/p");

        if (q_pos != std::string::npos) content_switch++;
        goto LOOP_OVER;
      }; //content_switch
      case 4: {
        content_switch = 0;
        if (thumb.empty()) break;
        o_stream << thumb + L"|" + content + L"\n";
        thumb = L"";
        break;
      }; //Finish
      }; //section

    }; //while
  }; //for loop


  
}; //main