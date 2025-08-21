#include <Urlmon.h>
#pragma comment(lib,"Urlmon.lib")

#include <fstream>
#include <string>
#include <queue>
#include <iostream>

inline std::wstring trim(std::wstring& str)
{
  str.erase(str.find_last_not_of(' ') + 1);  // Suffixing spaces
  str.erase(0, str.find_first_not_of(' '));  // Prefixing spaces
  return str;
}


inline std::wifstream ScanSite(std::wstring url, std::wstring filename) {
  HRESULT res = URLDownloadToFile(NULL, url.c_str(), filename.c_str(), 0, NULL);
  return std::wifstream(filename);
}; //ScanSite

int main() {
  std::queue<std::wstring> url_q = {};
  std::wstring file_content;
  std::wstring file = L"html_file.html";
  std::wstring filter = L"<th><a";
  
  auto o_stream = std::wofstream("deadlockdeck.csv");

  uint8_t mode = 0;

  URLDownloadToFile(NULL, L"https://deadlock.wiki/Item", file.c_str(), 0, NULL);
  auto stream = std::wifstream(file);

  while (std::getline(stream, file_content)) {
    if (!(file_content.find(filter) == std::string::npos)) {

      auto quote_pos = file_content.find('"');
      auto second_quote = file_content.find(L"title");
      auto url_str = file_content.substr(quote_pos + 1, second_quote - quote_pos-3);
      url_q.push(L"https://deadlock.wiki" + url_str);

    };
  }; //For every line in file

  std::wstring cheeseburger = L"";
  std::wstring thumbnail = L"";

  for (; !url_q.empty(); url_q.pop()) {
    filter = L"infobox-table item-infobox";
    cheeseburger = L"";
    thumbnail = L"";
    stream.close();
    auto&  front = url_q.front();
    auto val = URLDownloadToFile(NULL, front.c_str(), file.c_str(), 0, NULL);
    stream = std::wifstream(file);

    while (std::getline(stream, file_content)) {
      if (cheeseburger.empty()) {
        
        if (!(file_content.find(filter) == std::string::npos)) {
          cheeseburger += file_content;
          filter = L"</table>";

        };
      }
      else {
        if (!(file_content.find(L"srcset=") == std::string::npos)) {
          auto src = file_content.find(L"srcset=");
          file_content.insert(src + 8, L"https://deadlock.wiki");
          if (!(file_content.find(L"1.5x,") == std::string::npos)) {
            auto comma = file_content.find(L"1.5x,");
            file_content.insert(comma + 6, L"https://deadlock.wiki");
          }
        }

        if (!(file_content.find(L"src=") == std::string::npos)) {
          auto src = file_content.find(L"src=");
          file_content.insert(src+5,L"https://deadlock.wiki");
          if (!(file_content.find(L"mw-file-element") == std::string::npos) && thumbnail.empty()) thumbnail = file_content;
        }

        if (!(file_content.find(L"href=") == std::string::npos)) {
          auto src = file_content.find(L"href=");
          file_content.insert(src + 6, L"https://deadlock.wiki");
        }


        cheeseburger += file_content;
        if (!(file_content.find(filter) == std::string::npos)) goto EXIT_ITEM_LOOP;

      }; //else
      
      
    }; //For every line in file

    EXIT_ITEM_LOOP:
    o_stream << thumbnail + L"|" + cheeseburger + L"\n";
  }; //for loop

  //for (; !url_q.empty(); url_q.pop()) {
  //  stream.close();
  //  auto& front = url_q.front();

  //  auto val = URLDownloadToFile(NULL, front.c_str(), file.c_str(), 0, NULL);
  //  stream = std::wifstream(file);

  //  filter = L"<div class=\"grid-box-bikez\">";
  //  mode = 0;

  //  std::wstring csv_string;

  //  while (std::getline(stream, file_content)) {

  //    if (!(file_content.find(filter) == std::string::npos)) {
  //      switch (mode) {
  //        case 0: {
  //          filter = L"<img src="; ++mode;
  //        } goto EXIT_SWITCH;
  //        case 1: {
  //          auto quote_pos = file_content.find(L'"');
  //          auto second_quote = file_content.find(L"alt=");
  //          csv_string += file_content.substr(0, second_quote -1);
  //          csv_string += L"/>";
  //          csv_string += L",";
  //          filter = L"<td style=\"vertical-align:top;\">"; ++mode;
  //        } goto EXIT_SWITCH;
  //        case 2: {
  //          filter = L"</td>";
  //          ++mode;
  //        } goto EXIT_SWITCH;
  //        case 3: {
  //          auto pos = file_content.find(L"</td>");
  //          auto substr = file_content.substr(0, pos);
  //          trim(substr);
  //          csv_string += substr;
  //          csv_string += L"\n";

  //        } goto EXIT_LOOP;
  //      }; //different filters
  //    }; //switch
  //     
  //  EXIT_SWITCH:
  //    std::wcout << L"Building CSV string" + csv_string + L"\n";

  //  }; //For every line in file

  //  EXIT_LOOP:
  //  o_stream << csv_string;
  //}; //for loop

  o_stream.close();

  return 0;
}; //main