#include <Urlmon.h>
#pragma comment(lib,"Urlmon.lib")

#include <fstream>
#include <array>
#include <string>
#include <queue>
#include <iostream>

int main() {
  std::queue<std::wstring> restaurant_q;
  std::wstring html_file = L"restaurant.html";
  std::wstring csv_file = L"restaurant.html";
  std::wstring file_content = L"";
  std::wstring url = L"https://guide.michelin.com/us/en/restaurants";
  std::wstring full_url = L"https://guide.michelin.com/us/en/restaurants";
  uint32_t page_number = 1;
  
  auto o_stream = std::wofstream("deadlockdeck.csv");

  uint32_t last_size = 0;

  Next_URL:
  full_url = std::wstring(url + L"/page/" + std::to_wstring(page_number));
  URLDownloadToFile(NULL, full_url.c_str(), html_file.c_str(), 0, NULL);
  auto stream = std::wifstream(html_file);

  std::wcout << L"Scanning URL: " + full_url << "\n";
  
  uint32_t mark = 0;
  uint32_t page_count = 0;

  while (std::getline(stream, file_content)) {
  
    if (file_content.find(L"<!-- Start Restaurants Empty Result -->") != std::string::npos) { mark = 0; break; };
    
    switch (mark) {
    case 0: {
      if (file_content.find(L"card__menu-image--bottom") == std::string::npos) { continue; }
      else ++mark;
    } continue;
    case 1: {
      if (file_content.find(L"/us/en/search") != std::string::npos) { mark = 0; continue; };
      if (file_content.find(L"/us/en/hotels-stays") != std::string::npos) { mark = 0; continue; };
      if (file_content.find(L"aria-label=Open Credance") != std::string::npos) { mark = 0; continue; };
      if (file_content.find(L"/us/en/") != std::string::npos) {
        ++page_count;
        
        auto quote = file_content.find(L"\"");
        file_content = file_content.erase(0, quote+1);
        quote = file_content.find(L"\"");
        file_content = file_content.erase(quote, file_content.size()-quote+1);
        file_content = L"              https://guide.michelin.com" + file_content;

        std::wcout << L"Grabbed URL: " + std::to_wstring(page_count) + file_content << "\n";
        restaurant_q.push(file_content);
        mark = 0;
      }; //if 

    }; //case 1
    }; //switch
  
  }; //whileloop
  
  page_count = 0;
  ++page_number;
  if (restaurant_q.size()-last_size < 45) goto QUEUE_LABEL;
  last_size = restaurant_q.size();
  goto Next_URL;
    
  QUEUE_LABEL:
  std::wstring output;
  std::wstring classification;

  uint64_t mode = 0;
  uint64_t div_count = 0;
  uint64_t div_close_count = 0;
  for (; !restaurant_q.empty(); restaurant_q.pop()) {
    mode = 0;
    div_count = 0;
    div_close_count = 0;
    
    classification = L"";
    output = L"";
    auto& front = restaurant_q.front();
    stream.close();

    URLDownloadToFile(NULL, front.c_str(), html_file.c_str(), 0, NULL);
    
    stream = std::wifstream(html_file);

      NEXT_LINE:
    while (std::getline(stream, file_content)) {

      switch (mode) {
        default: {
          div_close_count = 0;
          div_count = 0;
          if (file_content.find(L"masthead__gallery-image") != std::string::npos) { output += file_content; ++mode; div_count += 1; };

        } goto NEXT_LINE;
        case 1: {
          if (file_content.find(L"/div") != std::string::npos) ++div_close_count;
          else if (file_content.find(L"div") != std::string::npos) ++div_count;
          
          auto ci_src = file_content.find(L"ci-src");
          if (ci_src != std::string::npos) file_content = file_content.erase(ci_src, 3);
          output += file_content;
          if (div_close_count && div_close_count == div_count) { output += L"|";  ++mode; };
        }goto NEXT_LINE; //case 1

        case 2: {
          if (file_content.find(L"<h1 class=\"data-sheet__title\">") != std::string::npos)
            output += file_content + L"|";

          div_close_count = 0;
          div_count = 0;
          if (file_content.find(L"data-sheet__detail-info") != std::string::npos) { output += file_content; ++mode; };
        
        } goto NEXT_LINE;
        case 3: {
          if (file_content.find(L"/div") != std::string::npos) ++div_close_count;
          else if (file_content.find(L"div") != std::string::npos) ++div_count;

          output += file_content;
          if (div_close_count && div_close_count == div_count) { output += L"|";  ++mode; };
        }goto NEXT_LINE; //case 1


        case 4: {
          div_close_count = 0;
          div_count = 0;
          if (file_content.find(L"data-sheet__classification") != std::string::npos) { classification += file_content; mode = 5; };
          if (file_content.find(L"data-sheet__description ") != std::string::npos) { output += file_content; mode = 6; div_count += 1;};
        } goto NEXT_LINE; //case 2
        case 5: {
          if (file_content.find(L"/div") != std::string::npos) ++div_close_count;
          else if (file_content.find(L"div") != std::string::npos) ++div_count;

          classification += file_content;
          if (div_close_count && div_close_count == div_count) { mode=4; };
        }goto NEXT_LINE; //case 3


        case 6: {
          if (file_content.find(L"/div") != std::string::npos) ++div_close_count;
          else if (file_content.find(L"div") != std::string::npos) ++div_count;

          output += file_content;
          if (div_close_count && div_close_count == div_count) { ++mode; };
        }goto NEXT_LINE; //case 1


      }

    }; //for every line
    
    END_PAGE:
    o_stream << output << L"|" << classification << "\n";
  }

  return 0;
}