#include <Urlmon.h>
#pragma comment(lib,"Urlmon.lib")

#include <fstream>
#include <array>
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

  stream.close();
  
  file = L"h_file.html";
  auto val = URLDownloadToFile(NULL, L"https://deadlock.wiki/Hero", file.c_str(), 0, NULL);
  

  stream = std::wifstream(file);

  filter = L"<div style=\"display: flex; flex-wrap: wrap; gap: 3px; align-items: center; justify-content: center; margin: auto;\">";
  while (std::getline(stream, file_content)) {
    if (!(file_content.find(filter) == std::string::npos)) break;
  }; //For every line in file

  std::getline(stream, file_content);


  std::wstring rcrd_str{};
  std::wstring url{};
  char c = 0;
  for (uint32_t i = 0; i < file_content.size(); ++i) {
    c = file_content[i];
    rcrd_str += c;
    if (rcrd_str.empty() && c == L'<') { continue; };
    if (!rcrd_str.empty() && c == L'>') { 
      auto str_start = rcrd_str.find(L"href=");
      if (str_start != rcrd_str.npos) {
        auto end = rcrd_str.find(L"title");
        auto new_url = L"https://deadlock.wiki" + rcrd_str.substr(str_start + 6, end - str_start - 8);
        rcrd_str = L"";
        if (new_url == url) continue;
        url = new_url;
        url_q.push(url);
      }; //npos
    }; //if rcrd_str
  }; //for_loop
  
  std::array<std::wstring, 4> ability_thumbnail{};
  std::wstring hero_thumbnail{};
  std::wstring weapon_thumbnail{};

  std::array<std::wstring, 4> ability_content{};
  std::wstring hero_content{};
  std::wstring weapon_general{};
  std::wstring weapon_stats{};

  mode = 0;
  for (; !url_q.empty(); url_q.pop()) {
    filter = L"<img alt=\"{{{BASEPAGENAME}}}\"";
    hero_content = L"";
    weapon_general = L"";
    weapon_stats = L"";
    ability_content = {};
    ability_thumbnail = {};
    hero_thumbnail = L"";
    weapon_thumbnail = L"";
    mode = 0;
    stream.close();
    auto& front = url_q.front();
    auto val = URLDownloadToFile(NULL, front.c_str(), file.c_str(), 0, NULL);
    stream = std::wifstream(file);

    RESET_LOOP:
    while (std::getline(stream, file_content)) {

        uint64_t f_content_mode = 0;
        uint64_t f_content_pos = 0;
        uint64_t f_substr_count = -1;
        
        while (true) {
          f_substr_count++;

          switch (f_content_mode) {
          case 0: {
            f_content_pos = file_content.find(L"srcset=", f_content_pos + f_substr_count);

            if (f_content_pos == file_content.npos) {
              ++f_content_mode;
              f_substr_count = -1;
              f_content_pos = 0;
            }
            else {
              file_content.insert(f_content_pos + 8, L"https://deadlock.wiki");
            };

          } continue;
          case 1: {
            f_content_pos = file_content.find(L"1.5x,", f_content_pos + f_substr_count);

            if (f_content_pos == file_content.npos) {
              ++f_content_mode;
              f_substr_count = -1;
              f_content_pos = 0;
            }
            else {
              file_content.insert(f_content_pos + 6, L"https://deadlock.wiki");
            };
          } continue;
          case 2: {
            f_content_pos = file_content.find(L"src=", f_content_pos + f_substr_count);
       
            if (f_content_pos == file_content.npos) {
              ++f_content_mode;
              f_substr_count = -1;
              f_content_pos = 0;
            } else file_content.insert(f_content_pos + 5, L"https://deadlock.wiki");

            


          } continue;
          case 3: {
            f_content_pos = file_content.find(L"href=", f_content_pos + f_substr_count);
            
            if (f_content_pos == file_content.npos) {
              ++f_content_mode;
              f_substr_count = -1;
              f_content_pos = 0;
            } else file_content.insert(f_content_pos + 6, L"https://deadlock.wiki");
          } continue;
          case 4: {
            f_content_pos = file_content.find(L"<noscript>", f_content_pos + f_substr_count);
            if (f_content_pos == file_content.npos) {
              ++f_content_mode;
              f_substr_count = -1;
              f_content_pos = 0;
            }
            else file_content.erase(f_content_pos, 10);


          } continue; //case 3
          case 5: {
            f_content_pos = file_content.find(L"</noscript>", f_content_pos + f_substr_count);
            if (f_content_pos != file_content.npos) file_content.erase(f_content_pos, 11);
            else ++f_content_mode;
          } continue; //case 4
          default: goto EXIT_LOOP;
          }; //f_conten_mode

        }

        EXIT_LOOP:
      if (!(file_content.find(filter) == std::string::npos)) {
        switch (mode) {
          default:
        case 0: { hero_thumbnail = file_content; filter = L"#d5903f"; ++mode; } goto RESET_LOOP;
        case 1: { 
          
          weapon_stats += L"<table>";
          weapon_stats += L"<tbody>";
          weapon_stats += L"<tr>";
          weapon_stats += file_content;
          weapon_stats += L"</th>";
          weapon_stats += L"</tr>";
          weapon_stats += L"<tr>";

          filter = L"td";

          ++mode; } goto RESET_LOOP;
        case 2: {
          weapon_stats += file_content;

          if (!(file_content.find(L"Falloff Range") == std::string::npos)) { 
            ++mode;
          }; //mode
          if (!(file_content.find(L"tr") == std::string::npos)) { weapon_stats += L"<tr>"; };

        } goto RESET_LOOP; //case 2
        case 3: {
          weapon_stats += file_content;
          if (!(file_content.find(L"</tr>") == std::string::npos)) {
            weapon_stats += file_content;
            weapon_stats += L"</tbody>";
            weapon_stats += L"</table>";
            filter = L"#FFEFD7";
            ++mode;
          }; //mode
        } goto RESET_LOOP;
        case 4: {

          hero_content += L"<table>";
          hero_content += L"<tbody>";
          hero_content += L"<tr>";
          hero_content += file_content;
          hero_content += L"</th>";
          hero_content += L"</tr>";
          hero_content += L"<tr>";

          filter = L"td";
          ++mode; } goto RESET_LOOP;
        case 5: {
          hero_content += file_content;

          if (!(file_content.find(L"Stamina") == std::string::npos)) {
            ++mode;
          }; //mode
          if (!(file_content.find(L"tr") == std::string::npos)) { weapon_stats += L"<tr>"; };

        } goto RESET_LOOP; //case 5
        case 6: {
          hero_content += file_content;
          if (!(file_content.find(L"</tr>") == std::string::npos)) {
            hero_content += file_content;
            filter = L"#FFEFD7";
            ++mode;
          }; //mode
        } goto RESET_LOOP;
        case 7: {

          hero_content += L"<tr>";
          hero_content += file_content;
          hero_content += L"</th>";
          hero_content += L"</tr>";
          hero_content += L"<tr>";

          filter = L"td";
          ++mode;
        } goto RESET_LOOP;
        case 8: {
          hero_content += file_content;

          if (!(file_content.find(L"/table") == std::string::npos)) {
            ++mode;
            filter = L"tabbertab";
          }; //mode
          if (!(file_content.find(L"tr") == std::string::npos)) { hero_content += L"<tr>"; };

        } goto RESET_LOOP; //case 8
        case 9: {
          static uint32_t tab = 0;
          if (!(file_content.find(L"tabbertab") == std::string::npos)) tab++;
          if (tab > 3) tab = 0;

          filter = L"";
          ability_content[tab] += file_content;

          if (ability_thumbnail[tab].empty() && !(file_content.find(L"img") == std::string::npos)) {
            auto src = file_content.find(L"invert");
            ability_thumbnail[tab] = file_content;
            ability_thumbnail[tab].replace(src,11,L"");
          };

          if (!(file_content.find(L"edit") == std::string::npos)) {
            ++mode;
            filter = L"#180E02";
          }; //mode
          if (!(file_content.find(L"tr") == std::string::npos)) { hero_content += L"<tr>"; };

        } goto RESET_LOOP; //case 8
        case 10: {
          if (!(file_content.find(L"h2") == std::string::npos)) {
            if (!(file_content.find(L"<p>") == std::string::npos)) weapon_general += L"</p>";
            goto EXIT_HERO_LOOP;
          }
          weapon_general += file_content;
          filter = L"";

          if (!(file_content.find(L"img") == std::string::npos)) {
            weapon_thumbnail += L"<table>";
            weapon_thumbnail += L"<tbody>";
            weapon_thumbnail += file_content;
            weapon_thumbnail += L"</image>";
            weapon_thumbnail += L"</td>";
            weapon_thumbnail += L"</tbody>";
            weapon_thumbnail += L"</table>";

          }

        }; //case 9
        };
      }; //file_content


    }; //For every line in file

    EXIT_HERO_LOOP:
    o_stream << weapon_thumbnail + L"|" + weapon_general + weapon_stats + L"\n";
    o_stream << hero_thumbnail + L"|" + hero_content + L"\n";
    o_stream << ability_thumbnail[0] + L"|" + ability_content[0] + L"\n";
    o_stream << ability_thumbnail[1] + L"|" + ability_content[1] + L"\n";
    o_stream << ability_thumbnail[2] + L"|" + ability_content[2] + L"\n";
    o_stream << ability_thumbnail[3] + L"|" + ability_content[3] + L"\n";
  }; //for loop

  o_stream.close();

  return 0;
}; //main