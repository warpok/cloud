/*
Copyright (©) 2003-2016 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include <unittests/databases.h>
#include <unittests/utilities.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/roles.h>
#include <filter/md5.h>
#include <filter/date.h>
#include <filter/shell.h>
#include <config/globals.h>
#include <database/config/general.h>
#include <database/config/bible.h>
#include <database/logs.h>
#include <database/sqlite.h>
#include <database/users.h>
#include <database/books.h>
#include <database/bibleactions.h>
#include <database/check.h>
#include <database/localization.h>
#include <database/confirm.h>
#include <database/ipc.h>
#include <database/jobs.h>
#include <database/kjv.h>
#include <database/oshb.h>
#include <database/sblgnt.h>
#include <database/sprint.h>
#include <database/mail.h>
#include <database/navigation.h>
#include <database/usfmresources.h>
#include <database/mappings.h>
#include <database/noteactions.h>
#include <database/versifications.h>
#include <database/modifications.h>
#include <database/notes.h>
#include <database/volatile.h>
#include <database/state.h>
#include <database/imageresources.h>
#include <database/noteassignment.h>
#include <database/strong.h>
#include <database/morphgnt.h>
#include <database/etcbc4.h>
#include <database/hebrewlexicon.h>
#include <database/cache.h>
#include <database/login.h>
#include <database/privileges.h>
#include <database/git.h>
#include <database/userresources.h>
#include <database/statistics.h>
#include <bible/logic.h>
#include <notes/logic.h>
#include <sync/logic.h>
#include <styles/logic.h>
#include <resource/external.h>
#include <changes/logic.h>
#include <demo/logic.h>


void test_database_versifications ()
{
  trace_unit_tests (__func__);
  
  // Basic operations, create, delete.
  {
    refresh_sandbox (true);
    Database_Versifications database_versifications;
    database_versifications.create ();
    database_versifications.optimize ();
    int id = database_versifications.createSystem ("phpunit");
    evaluate (__LINE__, __func__, 1000, id);
    id = database_versifications.getID ("phpunit");
    evaluate (__LINE__, __func__, 1000, id);
    vector <string> systems = database_versifications.getSystems ();
    evaluate (__LINE__, __func__, {"phpunit"}, systems);
    database_versifications.erase ("phpunit");
    systems = database_versifications.getSystems ();
    evaluate (__LINE__, __func__, {}, systems);
  }
  {
    refresh_sandbox (true);
    Database_Versifications database_versifications;
    database_versifications.create ();
    database_versifications.defaults ();

    // GetID
    int id = database_versifications.getID (english ());
    evaluate (__LINE__, __func__, 4 , id);

    // Test books.
    vector <int> books = database_versifications.getBooks (english ());
    vector <int> standard;
    for (unsigned int i = 1; i <= 66; i++) standard.push_back (i);
    evaluate (__LINE__, __func__, standard, books);

    // Test chapters.
    vector <int> chapters = database_versifications.getChapters (english (), 1);
    standard.clear ();
    for (unsigned int i = 1; i <= 50; i++) standard.push_back (i);
    evaluate (__LINE__, __func__, standard, chapters);
    chapters = database_versifications.getChapters (english (), 1, true);
    standard.clear ();
    for (unsigned int i = 0; i <= 50; i++) standard.push_back (i);
    evaluate (__LINE__, __func__, standard, chapters);

    // Test verses.
    vector <int> verses = database_versifications.getVerses (english (), 1, 2);
    standard.clear ();
    for (unsigned int i = 0; i <= 25; i++) standard.push_back (i);
    evaluate (__LINE__, __func__, standard, verses);

    // Verses in chapter 0.
    verses = database_versifications.getVerses (english (), 1, 0);
    evaluate (__LINE__, __func__, {0}, verses);

    // Books Chapters Verses.
    vector <Passage> data = database_versifications.getBooksChaptersVerses (english ());
    evaluate (__LINE__, __func__, 1189, (int)data.size());
    evaluate (__LINE__, __func__, "31", data [0].verse);
    
    // Maximum number of books.
    books = database_versifications.getMaximumBooks ();
    standard = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 70, 71, 73, 74, 75, 80, 81 };
    evaluate (__LINE__, __func__, standard, books);
    
    // Maximum number of chapters.
    chapters = database_versifications.getMaximumChapters (5);
    standard.clear ();
    for (unsigned int i = 0; i <= 34; i++) standard.push_back (i);
    evaluate (__LINE__, __func__, standard, chapters);

    // Maximum number of verses.
    verses = database_versifications.getMaximumVerses (1, 2);
    standard.clear ();
    for (unsigned int i = 0; i <= 25; i++) standard.push_back (i);
    evaluate (__LINE__, __func__, standard, verses);
  }
  // Import Export
  {
    refresh_sandbox (true);
    Database_Versifications database_versifications;
    database_versifications.create ();
    string input = 
      "Genesis 1:31\n"
      "Genesis 2:25\n";
    database_versifications.input (input, "phpunit");
    int id = database_versifications.getID ("phpunit");
    evaluate (__LINE__, __func__, 1000, id);
    vector <Passage> data = database_versifications.getBooksChaptersVerses ("phpunit");
    evaluate (__LINE__, __func__, 2, (int)data.size ());
    evaluate (__LINE__, __func__, "25", data [1].verse);
    string output = database_versifications.output ("phpunit");
    evaluate (__LINE__, __func__, filter_string_trim (input), filter_string_trim (output));
  }
}


void test_database_modifications_user ()
{
  trace_unit_tests (__func__);
  
  // Create, erase, clear.
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.erase ();
    database_modifications.create ();
    database_modifications.clearUserUser ("phpunit");
  }
  // Usernames
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    vector <string> users = database_modifications.getUserUsernames ();
    evaluate (__LINE__, __func__, {}, users);
    database_modifications.recordUserSave ("phpunit1", "bible", 1, 2, 3, "old", 4, "new");
    users = database_modifications.getUserUsernames ();
    evaluate (__LINE__, __func__, {"phpunit1"}, users);
    database_modifications.recordUserSave ("phpunit2", "bible", 1, 2, 3, "old", 5, "new");
    database_modifications.recordUserSave ("phpunit3", "bible", 1, 2, 3, "old", 6, "new");
    database_modifications.recordUserSave ("phpunit1", "bible", 1, 2, 3, "old", 7, "new");
    database_modifications.recordUserSave ("phpunit2", "bible", 1, 2, 3, "old", 8, "new");
    database_modifications.recordUserSave ("phpunit3", "bible", 1, 2, 3, "old", 9, "new");
    users = database_modifications.getUserUsernames ();
    evaluate (__LINE__, __func__, {"phpunit1", "phpunit2", "phpunit3"}, users);
  }
  // Bibles
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    vector <string> bibles = database_modifications.getUserBibles ("phpunit1");
    evaluate (__LINE__, __func__, {}, bibles);
    database_modifications.recordUserSave ("phpunit1", "bible1", 1, 2, 3, "old", 4, "new");
    database_modifications.recordUserSave ("phpunit1", "bible1", 1, 2, 3, "old", 5, "new");
    database_modifications.recordUserSave ("phpunit1", "bible1", 1, 2, 3, "old", 6, "new");
    database_modifications.recordUserSave ("phpunit1", "bible1", 1, 2, 3, "old", 7, "new");
    bibles = database_modifications.getUserBibles ("phpunit1");
    evaluate (__LINE__, __func__, {"bible1"}, bibles);
  }
  // Books
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    vector <int> books = database_modifications.getUserBooks ("phpunit1", "bible1");
    evaluate (__LINE__, __func__, {}, books);
    database_modifications.recordUserSave ("phpunit1", "bible1", 1, 2, 3, "old", 4, "new");
    database_modifications.recordUserSave ("phpunit1", "bible1", 2, 2, 3, "old", 5, "new");
    database_modifications.recordUserSave ("phpunit1", "bible1", 3, 2, 3, "old", 6, "new");
    database_modifications.recordUserSave ("phpunit1", "bible2", 4, 2, 3, "old", 7, "new");
    books = database_modifications.getUserBooks ("phpunit1", "bible1");
    evaluate (__LINE__, __func__, {1, 2, 3}, books);
  }
  // Chapters
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    vector <int> chapters = database_modifications.getUserChapters ("phpunit1", "bible1", 1);
    evaluate (__LINE__, __func__, {}, chapters);
    database_modifications.recordUserSave ("phpunit1", "bible1", 1, 2, 3, "old", 4, "new");
    database_modifications.recordUserSave ("phpunit1", "bible1", 1, 3, 3, "old", 5, "new");
    database_modifications.recordUserSave ("phpunit1", "bible1", 1, 4, 3, "old", 6, "new");
    chapters = database_modifications.getUserChapters ("phpunit1", "bible1", 1);
    evaluate (__LINE__, __func__, {2, 3, 4}, chapters);
  }
  // Identifiers.
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.recordUserSave ("phpunit1", "bible", 1, 2, 3, "old", 4, "new");
    database_modifications.recordUserSave ("phpunit1", "bible", 1, 2, 4, "old", 5, "new");
    database_modifications.recordUserSave ("phpunit1", "bible", 1, 2, 5, "old", 6, "new");
    vector <Database_Modifications_Id> identifiers = database_modifications.getUserIdentifiers ("phpunit1", "bible", 1, 2);
    evaluate (__LINE__, __func__, 3, (int)identifiers.size());
    evaluate (__LINE__, __func__, 3, identifiers[0].oldid);
    evaluate (__LINE__, __func__, 4, identifiers[0].newid);
    evaluate (__LINE__, __func__, 4, identifiers[1].oldid);
    evaluate (__LINE__, __func__, 5, identifiers[1].newid);
    evaluate (__LINE__, __func__, 5, identifiers[2].oldid);
    evaluate (__LINE__, __func__, 6, identifiers[2].newid);
  }
  // Chapter
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.recordUserSave ("phpunit1", "bible", 1, 2, 3, "old1", 4, "new1");
    database_modifications.recordUserSave ("phpunit1", "bible", 1, 2, 4, "old2", 5, "new2");
    database_modifications.recordUserSave ("phpunit1", "bible", 1, 2, 5, "old3", 6, "new3");
    Database_Modifications_Text chapter = database_modifications.getUserChapter ("phpunit1", "bible", 1, 2, 4);
    evaluate (__LINE__, __func__, "old1", chapter.oldtext);
    evaluate (__LINE__, __func__, "new1", chapter.newtext);
    chapter = database_modifications.getUserChapter ("phpunit1", "bible", 1, 2, 5);
    evaluate (__LINE__, __func__, "old2", chapter.oldtext);
    evaluate (__LINE__, __func__, "new2", chapter.newtext);
  }
  // Timestamp
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.recordUserSave ("phpunit1", "bible", 1, 2, 3, "old1", 4, "new1");
    int time = database_modifications.getUserTimestamp ("phpunit1", "bible", 1, 2, 4);
    int currenttime = filter_date_seconds_since_epoch ();
    if ((time < currenttime - 1) || (time > currenttime + 1)) evaluate (__LINE__, __func__, currenttime, time);
  }
}


void test_database_modifications_team ()
{
  trace_unit_tests (__func__);
  
  // Basics.
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();
    Database_State::create ();
    Database_Bibles database_bibles;
    database_bibles.createBible ("phpunit");
    database_bibles.createBible ("phpunit2");
  }
  // Team Existence
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();
    Database_State::create ();
    Database_Bibles database_bibles;
    database_bibles.createBible ("phpunit");
    database_bibles.createBible ("phpunit2");
 
    // Initially no diff should exist.
    bool exists = database_modifications.teamDiffExists ("phpunit", 1, 2);
    evaluate (__LINE__, __func__, false, exists);

    // After storing a chapter there should be a diff.
    bible_logic_store_chapter ("phpunit", 1, 2, "chapter text one");
    exists = database_modifications.teamDiffExists ("phpunit", 1, 2);
    evaluate (__LINE__, __func__, true, exists);

    // After storing a chapter for the second time, the diff should still exist.
    bible_logic_store_chapter ("none", 1, 2, "chapter text two");
    exists = database_modifications.teamDiffExists ("phpunit", 1, 2);
    evaluate (__LINE__, __func__, true, exists);
    
    // The diff data should not exist for another chapter.
    exists = database_modifications.teamDiffExists ("phpunit", 1, 1);
    evaluate (__LINE__, __func__, false, exists);
  }
  // Team Book
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();
    Database_State::create ();
    Database_Bibles database_bibles;
    database_bibles.createBible ("phpunit");
    database_bibles.createBible ("phpunit2");

    evaluate (__LINE__, __func__, false, database_modifications.teamDiffExists ("phpunit", 2, 1));
    evaluate (__LINE__, __func__, false, database_modifications.teamDiffExists ("phpunit", 2, 2));
    evaluate (__LINE__, __func__, false, database_modifications.teamDiffExists ("phpunit", 2, 3));

    bible_logic_store_chapter ("phpunit", 2, 1, "chapter text");
    bible_logic_store_chapter ("phpunit", 2, 2, "chapter text");
    bible_logic_store_chapter ("phpunit", 2, 3, "chapter text");

    evaluate (__LINE__, __func__, true, database_modifications.teamDiffExists ("phpunit", 2, 1));
    evaluate (__LINE__, __func__, true, database_modifications.teamDiffExists ("phpunit", 2, 2));
    evaluate (__LINE__, __func__, true, database_modifications.teamDiffExists ("phpunit", 2, 3));

    database_modifications.truncateTeams ();

    // Test function storeTeamDiffBook.
    database_modifications.storeTeamDiffBook ("phpunit", 2);
    evaluate (__LINE__, __func__, true, database_modifications.teamDiffExists ("phpunit", 2, 1));
    evaluate (__LINE__, __func__, true, database_modifications.teamDiffExists ("phpunit", 2, 2));
    evaluate (__LINE__, __func__, true, database_modifications.teamDiffExists ("phpunit", 2, 3));
  }
  // Team Bible
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();
    Database_State::create ();
    Database_Bibles database_bibles;
    database_bibles.createBible ("phpunit");
    database_bibles.createBible ("phpunit2");

    evaluate (__LINE__, __func__, false, database_modifications.teamDiffExists ("phpunit", 3, 1));
    evaluate (__LINE__, __func__, false, database_modifications.teamDiffExists ("phpunit", 4, 1));
    evaluate (__LINE__, __func__, false, database_modifications.teamDiffExists ("phpunit", 5, 1));

    bible_logic_store_chapter ("phpunit", 3, 1, "chapter text");
    bible_logic_store_chapter ("phpunit", 4, 1, "chapter text");
    bible_logic_store_chapter ("phpunit", 5, 1, "chapter text");

    evaluate (__LINE__, __func__, true, database_modifications.teamDiffExists ("phpunit", 3, 1));
    evaluate (__LINE__, __func__, true, database_modifications.teamDiffExists ("phpunit", 4, 1));
    evaluate (__LINE__, __func__, true, database_modifications.teamDiffExists ("phpunit", 5, 1));

    database_modifications.truncateTeams ();

    // Test function storeTeamDiffBible. It stores diff data for the whole bible.
    database_modifications.storeTeamDiffBible ("phpunit");

    evaluate (__LINE__, __func__, true, database_modifications.teamDiffExists ("phpunit", 3, 1));
    evaluate (__LINE__, __func__, true, database_modifications.teamDiffExists ("phpunit", 4, 1));
    evaluate (__LINE__, __func__, true, database_modifications.teamDiffExists ("phpunit", 5, 1));
  }
  // Team Get Diff
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();
    Database_State::create ();
    Database_Bibles database_bibles;
    database_bibles.createBible ("phpunit");
    database_bibles.createBible ("phpunit2");

    string diff = database_modifications.getTeamDiff ("phpunit", 1, 1);
    evaluate (__LINE__, __func__, "", diff);

    bible_logic_store_chapter ("phpunit", 3, 1, "chapter text");
    database_modifications.truncateTeams ();
    bible_logic_store_chapter ("phpunit", 3, 1, "longer chapter text");
    diff = database_modifications.getTeamDiff ("phpunit", 3, 1);
    evaluate (__LINE__, __func__, "chapter text", diff);

    diff = database_modifications.getTeamDiff ("phpunit", 1, 2);
    evaluate (__LINE__, __func__, "", diff);

    bible_logic_store_chapter ("phpunit", 5, 5, "chapter text");
    database_modifications.truncateTeams ();
    bible_logic_store_chapter ("phpunit", 5, 5, "longer chapter text");
    diff = database_modifications.getTeamDiff ("phpunit", 5, 5);
    evaluate (__LINE__, __func__, "chapter text", diff);
  }
  // Team Get Diff Chapters
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();
    Database_State::create ();
    Database_Bibles database_bibles;
    database_bibles.createBible ("phpunit");
    database_bibles.createBible ("phpunit2");

    bible_logic_store_chapter ("phpunit", 3, 1, "chapter text");
    bible_logic_store_chapter ("phpunit", 3, 3, "chapter text");
    bible_logic_store_chapter ("phpunit", 3, 5, "chapter text");

    vector <int> chapters = database_modifications.getTeamDiffChapters ("phpunit", 1);
    evaluate (__LINE__, __func__, {}, chapters);

    chapters = database_modifications.getTeamDiffChapters ("phpunit", 3);
    evaluate (__LINE__, __func__, {1, 3, 5}, chapters);

    chapters = database_modifications.getTeamDiffChapters ("phpunit2", 3);
    evaluate (__LINE__, __func__, {}, chapters);
  }
  // Team Diff Bible
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();
    Database_State::create ();
    Database_Bibles database_bibles;
    database_bibles.createBible ("phpunit");
    database_bibles.createBible ("phpunit2");

    bible_logic_store_chapter ("phpunit", 3, 1, "chapter text");
    bible_logic_store_chapter ("phpunit", 3, 3, "chapter text");
    bible_logic_store_chapter ("phpunit", 3, 5, "chapter text");
    
    vector <string> bibles = database_modifications.getTeamDiffBibles ();
    evaluate (__LINE__, __func__, {"phpunit"}, bibles);
    
    database_modifications.deleteTeamDiffBible ("phpunit2");

    bibles = database_modifications.getTeamDiffBibles ();
    evaluate (__LINE__, __func__, {"phpunit"}, bibles);

    database_modifications.deleteTeamDiffBible ("phpunit");

    bibles = database_modifications.getTeamDiffBibles ();
    evaluate (__LINE__, __func__, {}, bibles);
  }
  // Team Diff Chapter.
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();
    Database_State::create ();
    Database_Bibles database_bibles;
    database_bibles.createBible ("phpunit");
    database_bibles.createBible ("phpunit2");

    bible_logic_store_chapter ("phpunit", 3, 1, "chapter text");
    bible_logic_store_chapter ("phpunit", 3, 3, "chapter text");
    bible_logic_store_chapter ("phpunit", 3, 5, "chapter text");
    
    vector <int> chapters = database_modifications.getTeamDiffChapters ("phpunit", 3);
    evaluate (__LINE__, __func__, {1, 3, 5}, chapters);

    database_modifications.deleteTeamDiffChapter ("phpunit", 3, 1);

    chapters = database_modifications.getTeamDiffChapters ("phpunit", 3);
    evaluate (__LINE__, __func__, {3, 5}, chapters);
  }
  // Team Diff Book
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();
    Database_State::create ();
    Database_Bibles database_bibles;
    database_bibles.createBible ("phpunit");
    database_bibles.createBible ("phpunit2");

    bible_logic_store_chapter ("phpunit", 3, 1, "chapter text");
    bible_logic_store_chapter ("phpunit", 3, 3, "chapter text");
    bible_logic_store_chapter ("phpunit", 4, 5, "chapter text");

    vector <int> books = database_modifications.getTeamDiffBooks ("phpunit");
    evaluate (__LINE__, __func__, {3, 4}, books);

    books = database_modifications.getTeamDiffBooks ("phpunit2");
    evaluate (__LINE__, __func__, {}, books);
  }
  // Get Team Diff Count
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();
    Database_State::create ();
    Database_Bibles database_bibles;
    database_bibles.createBible ("phpunit");
    database_bibles.createBible ("phpunit2");

    bible_logic_store_chapter ("phpunit", 3, 1, "chapter text");
    bible_logic_store_chapter ("phpunit2", 3, 3, "chapter text");
    bible_logic_store_chapter ("phpunit2", 3, 5, "chapter text");

    int count = database_modifications.getTeamDiffCount ("phpunit");
    evaluate (__LINE__, __func__, 1, count);

    count = database_modifications.getTeamDiffCount ("phpunit2");
    evaluate (__LINE__, __func__, 2, count);

    count = database_modifications.getTeamDiffCount ("phpunit3");
    evaluate (__LINE__, __func__, 0, count);
  }
}


void test_database_modifications_notifications ()
{
  trace_unit_tests (__func__);
  
  // Basics.
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();
    vector <int> ids = database_modifications.getNotificationIdentifiers ();
    for (auto id : ids) {
      database_modifications.deleteNotification (id);
    }
  }
  // Trim.
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();

    // Record two entries.
    database_modifications.recordNotification ({"phpunit1", "phpunit2"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    database_modifications.indexTrimAllNotifications ();
    vector <int> ids = database_modifications.getNotificationIdentifiers ();
    evaluate (__LINE__, __func__, {1, 2}, ids);
    
    // After trimming the two entries should still be there.
    database_modifications.indexTrimAllNotifications ();
    ids = database_modifications.getNotificationIdentifiers ();
    evaluate (__LINE__, __func__, {1, 2}, ids);

    // Set the time back, re-index, filter_string_trim, and check one entry's gone.
    database_modifications.indexTrimAllNotifications ();
    database_modifications.notificationUpdateTime (1, filter_date_seconds_since_epoch () - 7776001);
    database_modifications.indexTrimAllNotifications ();
    ids = database_modifications.getNotificationIdentifiers ();
    evaluate (__LINE__, __func__, {2}, ids);
  }
  // Next Identifier.
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();
    int identifier = database_modifications.getNextAvailableNotificationIdentifier ();
    evaluate (__LINE__, __func__, 1, identifier);
    database_modifications.recordNotification ({"phpunit1"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    identifier = database_modifications.getNextAvailableNotificationIdentifier ();
    evaluate (__LINE__, __func__, 2, identifier);
    database_modifications.deleteNotification (1);
    identifier = database_modifications.getNextAvailableNotificationIdentifier ();
    evaluate (__LINE__, __func__, 1, identifier);
  }
  // Record Details Retrieval.
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();

    // Start with no identifiers.
    vector <int> ids = database_modifications.getNotificationIdentifiers ();
    evaluate (__LINE__, __func__, {}, ids);

    // Record three notifications and reindex.
    database_modifications.recordNotification ({"phpunit1", "phpunit2"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    database_modifications.recordNotification ({"phpunit2", "phpunit3"}, "A", "1", 4, 5, 6, "old2", "mod2", "new2");
    database_modifications.recordNotification ({"phpunit3", "phpunit4"}, changes_bible_category (), "1", 7, 8, 9, "old3", "mod3", "new3");
    database_modifications.indexTrimAllNotifications ();

    // There should be six notifications now: Two users per recordNotification call.
    ids = database_modifications.getNotificationIdentifiers ();
    evaluate (__LINE__, __func__, {1, 2, 3, 4, 5, 6}, ids);
    
    // Test notifications per user.
    ids = database_modifications.getNotificationIdentifiers ("phpunit1");
    evaluate (__LINE__, __func__, {1}, ids);
    ids = database_modifications.getNotificationIdentifiers ("phpunit3");
    evaluate (__LINE__, __func__, {4, 5}, ids);
  }
  // Timestamps
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();

    int timestamp = database_modifications.getNotificationTimeStamp (0);
    int currenttime = filter_date_seconds_since_epoch ();
    if ((timestamp < currenttime) || (timestamp > currenttime + 1)) evaluate (__LINE__, __func__, currenttime, timestamp);

    int time = filter_date_seconds_since_epoch () - 21600;
    database_modifications.recordNotification ({"phpunit"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    database_modifications.indexTrimAllNotifications ();
    timestamp = database_modifications.getNotificationTimeStamp (1);
    if ((timestamp < time) || (timestamp > time + 1)) evaluate (__LINE__, __func__, time, timestamp);
  }
  // Category
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();
    database_modifications.recordNotification ({"phpunit"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    database_modifications.indexTrimAllNotifications ();
    string category = database_modifications.getNotificationCategory (1);
    evaluate (__LINE__, __func__, "A", category);
    category = database_modifications.getNotificationCategory (2);
    evaluate (__LINE__, __func__, "", category);
  }
  // Bible.
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();
    database_modifications.recordNotification ({"phpunit"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    database_modifications.indexTrimAllNotifications ();
    string bible = database_modifications.getNotificationBible (1);
    evaluate (__LINE__, __func__, "1", bible);
    bible = database_modifications.getNotificationBible (3);
    evaluate (__LINE__, __func__, "", bible);
  }
  // Passage
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();
    database_modifications.recordNotification ({"phpunit"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    database_modifications.indexTrimAllNotifications ();
    Passage passage = database_modifications.getNotificationPassage (1);
    evaluate (__LINE__, __func__, 1, passage.book);
    evaluate (__LINE__, __func__, 2, passage.chapter);
    evaluate (__LINE__, __func__, "3", passage.verse);
    passage = database_modifications.getNotificationPassage (3);
    evaluate (__LINE__, __func__, 0, passage.book);
    evaluate (__LINE__, __func__, 0, passage.chapter);
    evaluate (__LINE__, __func__, "", passage.verse);
  }
  // OldText
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();
    database_modifications.recordNotification ({"phpunit"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    database_modifications.indexTrimAllNotifications ();
    string old = database_modifications.getNotificationOldText (1);
    evaluate (__LINE__, __func__, "old1", old);
    old = database_modifications.getNotificationOldText (3);
    evaluate (__LINE__, __func__, "", old);
  }
  // Modification.
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();
    database_modifications.recordNotification ({"phpunit"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    database_modifications.indexTrimAllNotifications ();
    string modification = database_modifications.getNotificationModification (1);
    evaluate (__LINE__, __func__, "mod1", modification);
    modification = database_modifications.getNotificationOldText (3);
    evaluate (__LINE__, __func__, "", modification);
  }
  // New Text
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();
    database_modifications.recordNotification ({"phpunit"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    database_modifications.indexTrimAllNotifications ();
    string newtext = database_modifications.getNotificationNewText (1);
    evaluate (__LINE__, __func__, "new1", newtext);
    newtext = database_modifications.getNotificationNewText (3);
    evaluate (__LINE__, __func__, "", newtext);
  }
  // Clear User
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();
    database_modifications.recordNotification ({"phpunit1", "phpunit2", "phpunit3"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    database_modifications.indexTrimAllNotifications ();
    vector <int> ids = database_modifications.getNotificationIdentifiers ();
    evaluate (__LINE__, __func__, 3, (int)ids.size ());

    database_modifications.clearNotificationsUser ("phpunit2");

    ids = database_modifications.getNotificationIdentifiers ();
    evaluate (__LINE__, __func__, 2, (int)ids.size ());

    ids = database_modifications.getNotificationIdentifiers ("phpunit2");
    evaluate (__LINE__, __func__, 0, (int)ids.size ());
  }
  // Clear Matches One
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();
    database_modifications.recordNotification ({"phpunit"}, changes_personal_category (), "1", 2, 3, 4, "old1", "mod1", "new1");
    database_modifications.recordNotification ({"phpunit"}, "T", "1", 2, 3, 4, "old1", "mod1", "new1");
    database_modifications.indexTrimAllNotifications ();
    vector <int> ids = database_modifications.getNotificationIdentifiers ();
    evaluate (__LINE__, __func__, 2, (int)ids.size ());
    database_modifications.clearNotificationMatches ("phpunit", changes_personal_category (), "T");
    database_modifications.indexTrimAllNotifications ();
    ids = database_modifications.getNotificationIdentifiers ();
    evaluate (__LINE__, __func__, 0, (int)ids.size ());
  }
  // Notification Personal Identifiers
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();
    database_modifications.recordNotification ({"phpunit1", "phpunit2"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    database_modifications.recordNotification ({"phpunit2", "phpunit1"}, changes_bible_category (), "1", 1, 2, 3, "old2", "mod2", "new2");
    database_modifications.recordNotification ({"phpunit3", "phpunit4"}, changes_bible_category (), "1", 7, 8, 9, "old3", "mod3", "new3");
    database_modifications.indexTrimAllNotifications ();
    vector <int> ids = database_modifications.getNotificationPersonalIdentifiers ("phpunit1", "A");
    evaluate (__LINE__, __func__, {1, 4}, ids);
  }
  // Notification Team Identifiers
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();
    database_modifications.recordNotification ({"phpunit1", "phpunit2"}, "A", "1", 1, 2, 3, "old1", "mod1", "new1");
    database_modifications.recordNotification ({"phpunit2", "phpunit1"}, changes_bible_category (), "1", 1, 2, 3, "old2", "mod2", "new2");
    database_modifications.recordNotification ({"phpunit3", "phpunit4"}, changes_bible_category (), "1", 7, 8, 9, "old3", "mod3", "new3");
    database_modifications.indexTrimAllNotifications ();
    vector <int> ids = database_modifications.getNotificationTeamIdentifiers ("phpunit1", "A");
    evaluate (__LINE__, __func__, {1}, ids);
    ids = database_modifications.getNotificationTeamIdentifiers ("phpunit1", changes_bible_category ());
    evaluate (__LINE__, __func__, {4}, ids);
  }
  // Record on client.
  {
    refresh_sandbox (true);
    Database_Modifications database_modifications;
    database_modifications.create ();
    database_modifications.storeClientNotification (3, "phpunit", "A", "bible", 1, 2, 3, "old1", "mod1", "new1");
    database_modifications.storeClientNotification (5, "phpunit", "A", "bible", 1, 2, 3, "old1", "mod1", "new1");
    vector <int> ids = database_modifications.getNotificationIdentifiers ();
    evaluate (__LINE__, __func__, {3, 5}, ids);
  }
}


void test_database_volatile ()
{
  trace_unit_tests (__func__);
  
  // No value initially.
  string value = Database_Volatile::getValue (1, "key");
  evaluate (__LINE__, __func__, "", value);
  
  // Store value and retrieve it.
  Database_Volatile::setValue (2, "key2", "value2");
  value = Database_Volatile::getValue (2, "key2");
  evaluate (__LINE__, __func__, "value2", value);
  
  // Another key should retrieve nothing.
  value = Database_Volatile::getValue (2, "key1");
  evaluate (__LINE__, __func__, "", value);
}


void test_database_state ()
{
  trace_unit_tests (__func__);
  
  refresh_sandbox (true);
  Database_State::create ();
  // Test notes checksums.
  {
    // No checksum yet.
    evaluate (__LINE__, __func__, "",  Database_State::getNotesChecksum (100, 1000));
    
    // Store and retrieve checksum in a defined range.
    Database_State::putNotesChecksum (100, 1000, "phpunit");
    evaluate (__LINE__, __func__, "phpunit",  Database_State::getNotesChecksum (100, 1000));
    // Store it again, with a different value.
    Database_State::putNotesChecksum (100, 1000, "phpunit2");
    evaluate (__LINE__, __func__, "phpunit2",  Database_State::getNotesChecksum (100, 1000));
    
    // Erase a note within the defined range, which should erase that range.
    Database_State::eraseNoteChecksum (100);
    evaluate (__LINE__, __func__, "",  Database_State::getNotesChecksum (100, 1000));
    
    // Define a few ranges, store checksums, and erase one note within that range, and test it.
    Database_State::putNotesChecksum (100, 1000, "100-1000");
    Database_State::putNotesChecksum (200, 1100, "200-1100");
    Database_State::putNotesChecksum (300, 900,  "300-900");
    Database_State::putNotesChecksum (2000, 9000, "2000-9000");
    evaluate (__LINE__, __func__, "100-1000",   Database_State::getNotesChecksum (100,  1000));
    evaluate (__LINE__, __func__, "200-1100",   Database_State::getNotesChecksum (200,  1100));
    evaluate (__LINE__, __func__, "300-900",    Database_State::getNotesChecksum (300,  900));
    evaluate (__LINE__, __func__, "2000-9000",  Database_State::getNotesChecksum (2000, 9000));
    Database_State::eraseNoteChecksum (500);
    evaluate (__LINE__, __func__, "",   Database_State::getNotesChecksum (100,  1000));
    evaluate (__LINE__, __func__, "",   Database_State::getNotesChecksum (200,  1100));
    evaluate (__LINE__, __func__, "",    Database_State::getNotesChecksum (300,  900));
    evaluate (__LINE__, __func__, "2000-9000",  Database_State::getNotesChecksum (2000, 9000));
  }
  // Test export flagging.
  {
    // Flag some data for export.
    Database_State::setExport ("1", 2, 3);
    Database_State::setExport ("4", 5, 6);
    Database_State::setExport ("7", 8, 9);

    // Test the data.
    evaluate (__LINE__, __func__, true,  Database_State::getExport ("1", 2, 3));
    evaluate (__LINE__, __func__, true,  Database_State::getExport ("4", 5, 6));
    evaluate (__LINE__, __func__, false,  Database_State::getExport ("1", 2, 1));
    
    // Clear flag.
    Database_State::clearExport ("1", 2, 3);
    
    // Test the data.
    evaluate (__LINE__, __func__, false,  Database_State::getExport ("1", 2, 3));
    evaluate (__LINE__, __func__, true,  Database_State::getExport ("4", 5, 6));
    evaluate (__LINE__, __func__, false,  Database_State::getExport ("1", 2, 1));
  }
}


void test_database_imageresources ()
{
  trace_unit_tests (__func__);
  
  Database_ImageResources database_imageresources;
  string image = filter_url_create_root_path ("unittests", "tests", "Genesis-1-1-18.gif");

  // Empty
  {
    refresh_sandbox (true);
    vector <string> resources = database_imageresources.names ();
    evaluate (__LINE__, __func__, 0, resources.size());
  }
  
  // Create, names, erase.
  {
    refresh_sandbox (true);

    database_imageresources.create ("unittest");
    vector <string> resources = database_imageresources.names ();
    evaluate (__LINE__, __func__, 1, resources.size());
    bool hit = false;
    for (auto & resource : resources) if (resource == "unittest") hit = true;
    evaluate (__LINE__, __func__, true, hit);
    
    database_imageresources.erase ("none-existing");
    resources = database_imageresources.names ();
    evaluate (__LINE__, __func__, 1, resources.size());

    database_imageresources.erase ("unittest");
    resources = database_imageresources.names ();
    evaluate (__LINE__, __func__, 0, resources.size());
  }
  
  // Store, get, erase images.
  {
    refresh_sandbox (true);
    
    database_imageresources.create ("unittest");
  
    string path = "/tmp/unittest.jpg";
    filter_url_file_cp (image, path);
    database_imageresources.store ("unittest", path);
    filter_url_file_cp (image, path);
    database_imageresources.store ("unittest", path);
    filter_url_unlink (path);

    vector <string> images = database_imageresources.get ("unittest");
    evaluate (__LINE__, __func__, images, {"unittest.jpg", "unittest0.jpg"});
    
    database_imageresources.erase ("unittest", "unittest.jpg");

    images = database_imageresources.get ("unittest");
    evaluate (__LINE__, __func__, images, {"unittest0.jpg"});
  }
  // Assign passage and get image based on passage.
  {
    refresh_sandbox (true);
    
    database_imageresources.create ("unittest");

    for (int i = 10; i < 20; i++) {
      string image = "unittest" + convert_to_string (i) + ".jpg";
      string path = "/tmp/" + image;
      filter_url_file_cp (image, path);
      database_imageresources.store ("unittest", path);
      filter_url_unlink (path);
      database_imageresources.assign ("unittest", image, i, i, i, i, i, i+10);
    }
    
    vector <string> images = database_imageresources.get ("unittest", 11, 11, 13);
    evaluate (__LINE__, __func__, images, {"unittest11.jpg"});
    
    images = database_imageresources.get ("unittest", 11, 11, 100);
    evaluate (__LINE__, __func__, images, {});
  }
  // Assign passage to image, and retrieve it.
  {
    refresh_sandbox (true);
    
    database_imageresources.create ("unittest");
    
    string image = "unittest.jpg";
    string path = "/tmp/" + image;
    filter_url_file_cp (image, path);
    database_imageresources.store ("unittest", path);
    filter_url_unlink (path);
    database_imageresources.assign ("unittest", image, 1, 2, 0, 1, 2, 10);

    int book1, chapter1, verse1, book2, chapter2, verse2;
    database_imageresources.get ("unittest", "none-existing",
                                 book1, chapter1, verse1, book2, chapter2, verse2);
    evaluate (__LINE__, __func__, book1, 0);
    evaluate (__LINE__, __func__, chapter1, 0);

    database_imageresources.get ("unittest", image,
                                 book1, chapter1, verse1, book2, chapter2, verse2);
    evaluate (__LINE__, __func__, book1, 1);
    evaluate (__LINE__, __func__, chapter1, 2);
    evaluate (__LINE__, __func__, verse1, 0);
    evaluate (__LINE__, __func__, book2, 1);
    evaluate (__LINE__, __func__, chapter2, 2);
    evaluate (__LINE__, __func__, verse2, 10);
  }
}


void test_database_noteassignment ()
{
  trace_unit_tests (__func__);
  
  refresh_sandbox (true);
  Database_NoteAssignment database;

  bool exists = database.exists ("unittest");
  evaluate (__LINE__, __func__, false, exists);
  
  vector <string> assignees = database.assignees ("unittest");
  evaluate (__LINE__, __func__, {}, assignees);

  database.assignees ("unittest", {"one", "two"});
  assignees = database.assignees ("none-existing");
  evaluate (__LINE__, __func__, {}, assignees);

  exists = database.exists ("unittest");
  evaluate (__LINE__, __func__, true, exists);

  assignees = database.assignees ("unittest");
  evaluate (__LINE__, __func__, {"one", "two"}, assignees);
  
  database.assignees ("unittest", {"1", "2"});
  assignees = database.assignees ("unittest");
  evaluate (__LINE__, __func__, {"1", "2"}, assignees);
  
  exists = database.exists ("unittest", "1");
  evaluate (__LINE__, __func__, true, exists);
  exists = database.exists ("unittest", "none-existing");
  evaluate (__LINE__, __func__, false, exists);
}


void test_database_strong ()
{
  trace_unit_tests (__func__);
  
  Database_Strong database;

  string result = database.definition ("G0");
  evaluate (__LINE__, __func__, "", result);
  
  result = database.definition ("G1");
  int length_h = result.length ();

  evaluate (__LINE__, __func__, true, length_h > 100);

  vector <string> results = database.strong ("χρηστοσ");
  evaluate (__LINE__, __func__, 1, results.size ());
  if (!results.empty ()) {
    evaluate (__LINE__, __func__, "G5543", results[0]);
  }
}


void test_database_morphgnt ()
{
  trace_unit_tests (__func__);
  
  Database_MorphGnt database;
  
  vector <int> results;
  
  results = database.rowids (0, 1, 2);
  evaluate (__LINE__, __func__, 0, results.size ());

  results = database.rowids (20, 3, 4);
  evaluate (__LINE__, __func__, 0, results.size ());
  
  results = database.rowids (40, 5, 6);
  evaluate (__LINE__, __func__, 10, results.size ());
  
  results = database.rowids (66, 7, 8);
  evaluate (__LINE__, __func__, 16, results.size ());
}


void test_database_etcbc4 ()
{
  trace_unit_tests (__func__);
  
  Database_Etcbc4 database;
  
  vector <int> rowids = database.rowids (1, 1, 1);
  evaluate (__LINE__, __func__, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}, rowids);

  rowids = database.rowids (2, 3, 4);
  evaluate (__LINE__, __func__, {
    29690,
    29691,
    29692,
    29693,
    29694,
    29695,
    29696,
    29697,
    29698,
    29699,
    29700,
    29701,
    29702,
    29703,
    29704,
    29705,
    29706,
    29707,
    29708,
    29709,
    29710,
    29711
  }, rowids);
  
  string result;
  
  result = database.word (2);
  evaluate (__LINE__, __func__, "רֵאשִׁ֖ית", result);

  result = database.word (1001);
  evaluate (__LINE__, __func__, "טֹ֛וב", result);
  
  result = database.vocalized_lexeme (2);
  evaluate (__LINE__, __func__, "רֵאשִׁית", result);
  
  result = database.vocalized_lexeme (1001);
  evaluate (__LINE__, __func__, "טֹוב", result);

  result = database.consonantal_lexeme (2);
  evaluate (__LINE__, __func__, "ראשׁית", result);

  result = database.consonantal_lexeme (1001);
  evaluate (__LINE__, __func__, "טוב", result);
  
  result = database.gloss (2);
  evaluate (__LINE__, __func__, "beginning", result);
  
  result = database.gloss (1001);
  evaluate (__LINE__, __func__, "good", result);
  
  result = database.pos (2);
  evaluate (__LINE__, __func__, "subs", result);
  
  result = database.pos (1001);
  evaluate (__LINE__, __func__, "adjv", result);
  
  result = database.subpos (2);
  evaluate (__LINE__, __func__, "none", result);
  
  result = database.subpos (1001);
  evaluate (__LINE__, __func__, "none", result);
  
  result = database.gender (2);
  evaluate (__LINE__, __func__, "f", result);
  
  result = database.gender (1001);
  evaluate (__LINE__, __func__, "m", result);
  
  result = database.number (4);
  evaluate (__LINE__, __func__, "pl", result);
  
  result = database.number (1001);
  evaluate (__LINE__, __func__, "sg", result);
  
  result = database.person (3);
  evaluate (__LINE__, __func__, "p3", result);
  
  result = database.person (1001);
  evaluate (__LINE__, __func__, "NA", result);
  
  result = database.state (2);
  evaluate (__LINE__, __func__, "a", result);
  
  result = database.state (1001);
  evaluate (__LINE__, __func__, "a", result);
  
  result = database.tense (3);
  evaluate (__LINE__, __func__, "perf", result);
  
  result = database.tense (1001);
  evaluate (__LINE__, __func__, "NA", result);
  
  result = database.stem (3);
  evaluate (__LINE__, __func__, "qal", result);
  
  result = database.stem (1001);
  evaluate (__LINE__, __func__, "NA", result);
  
  result = database.phrase_function (2);
  evaluate (__LINE__, __func__, "Time", result);
  
  result = database.phrase_function (1001);
  evaluate (__LINE__, __func__, "PreC", result);
  
  result = database.phrase_type (2);
  evaluate (__LINE__, __func__, "PP", result);
  
  result = database.phrase_type (1001);
  evaluate (__LINE__, __func__, "AdjP", result);
  
  result = database.phrase_relation (2);
  evaluate (__LINE__, __func__, "NA", result);
  
  result = database.phrase_relation (1001);
  evaluate (__LINE__, __func__, "NA", result);
  
  result = database.phrase_a_relation (2);
  evaluate (__LINE__, __func__, "NA", result);
  
  result = database.phrase_a_relation (1001);
  evaluate (__LINE__, __func__, "NA", result);
  
  result = database.clause_text_type (2);
  evaluate (__LINE__, __func__, "?", result);
  
  result = database.clause_text_type (1001);
  evaluate (__LINE__, __func__, "NQ", result);
  
  result = database.clause_type (2);
  evaluate (__LINE__, __func__, "xQtX", result);
  
  result = database.clause_type (1001);
  evaluate (__LINE__, __func__, "AjCl", result);
  
  result = database.clause_relation (2);
  evaluate (__LINE__, __func__, "NA", result);
  
  result = database.clause_relation (1001);
  evaluate (__LINE__, __func__, "NA", result);
}


void test_database_hebrewlexicon ()
{
  trace_unit_tests (__func__);
  
  Database_HebrewLexicon database;
  string result;

  result = database.getaug ("1");
  evaluate (__LINE__, __func__, "aac", result);

  result = database.getaug ("10");
  evaluate (__LINE__, __func__, "aai", result);

  result = database.getbdb ("a.aa.aa");
  evaluate (__LINE__, __func__, 160, result.length ());
  
  result = database.getbdb ("a.ac.ac");
  evaluate (__LINE__, __func__, 424, result.length ());
  
  result = database.getmap ("aaa");
  evaluate (__LINE__, __func__, "a.aa.aa", result);
  
  result = database.getmap ("aaj");
  evaluate (__LINE__, __func__, "a.ac.af", result);
  
  result = database.getpos ("a");
  evaluate (__LINE__, __func__, "adjective", result);
  
  result = database.getpos ("x");
  evaluate (__LINE__, __func__, "indefinite pronoun", result);
  
  result = database.getstrong ("H0");
  evaluate (__LINE__, __func__, "", result);
  
  result = database.getstrong ("H1");
  evaluate (__LINE__, __func__, 303, result.length ());
  
  result = database.getstrong ("H2");
  evaluate (__LINE__, __func__, 149, result.length ());
}


void test_database_cache ()
{
  trace_unit_tests (__func__);
  
  refresh_sandbox (true);

  // Initially the database should not exist.
  bool exists = Database_Cache::exists ("");
  evaluate (__LINE__, __func__, false, exists);
  exists = Database_Cache::exists ("unittests");
  evaluate (__LINE__, __func__, false, exists);

  // Copy an old cache database in place.
  // It contains cached data in the old layout.
  // Test that it now exists and contains data.
  string testdatapath = filter_url_create_root_path ("unittests", "tests", "cache_resource_test.sqlite");
  string databasepath = filter_url_create_root_path ("databases",  "cache_resource_unittests.sqlite");
  string out_err;
  filter_shell_run ("cp " + testdatapath + " " + databasepath, out_err);
  size_t count = Database_Cache::count ("unittests");
  evaluate (__LINE__, __func__, 1, count);
  exists = Database_Cache::exists ("unittests", 8, 1, 16);
  evaluate (__LINE__, __func__, true, exists);
  string value = Database_Cache::retrieve ("unittests", 8, 1, 16);
  evaluate (__LINE__, __func__, "And Ruth said, Entreat me not to leave you, or to return from following you; for wherever you go, I will go, and wherever you lodge, I will lodge; your people shall be my people, and your God my God.", value);

  // Now remove the (old) cache and verify that it no longer exists or contains data.
  Database_Cache::remove ("unittests");
  exists = Database_Cache::exists ("unittests");
  evaluate (__LINE__, __func__, false, exists);
  count = Database_Cache::count ("unittests");
  evaluate (__LINE__, __func__, 0, count);
  
  // Create a cache for one book.
  Database_Cache::create ("unittests", 10);
  // It should exists for the correct book, but not for another book.
  exists = Database_Cache::exists ("unittests", 10);
  evaluate (__LINE__, __func__, true, exists);
  exists = Database_Cache::exists ("unittests", 11);
  evaluate (__LINE__, __func__, false, exists);
  // The cache should have one book.
  count = Database_Cache::count ("unittests");
  evaluate (__LINE__, __func__, 1, count);
  
  // Cache and retrieve value.
  Database_Cache::create ("unittests", 1);
  Database_Cache::cache ("unittests", 1, 2, 3, "cached");
  value = Database_Cache::retrieve ("unittests", 1, 2, 3);
  evaluate (__LINE__, __func__, "cached", value);
  
  // Book count check.
  count = Database_Cache::count ("unittests");
  evaluate (__LINE__, __func__, 2, count);
  
  // Cache does not exist for one passage, but does exist for the other passage.
  exists = Database_Cache::exists ("unittests", 1, 2, 4);
  evaluate (__LINE__, __func__, false, exists);
  exists = Database_Cache::exists ("unittests", 1, 2, 3);
  evaluate (__LINE__, __func__, true, exists);
  
  // Excercise book cache removal.
  Database_Cache::remove ("unittests");
  exists = Database_Cache::exists ("unittests", 1);
  evaluate (__LINE__, __func__, false, exists);
  
  // Excercise the file-based cache.
  {
    string url = "https://netbible.org/bible/1/2/3";
    string contents = "Bible contents";
    evaluate (__LINE__, __func__, false, database_filebased_cache_exists (url));
    evaluate (__LINE__, __func__, "", database_filebased_cache_get (url));
    database_filebased_cache_put (url, contents);
    evaluate (__LINE__, __func__, true, database_filebased_cache_exists (url));
    evaluate (__LINE__, __func__, contents, database_filebased_cache_get (url));
    database_cache_trim ();
  }

  // Excercise the ready-flag.
  {
    string bible = "ready";
    int book = 11;
    Database_Cache::create (bible, book);
    
    bool ready = Database_Cache::ready (bible, book);
    evaluate (__LINE__, __func__, false, ready);
    
    Database_Cache::ready (bible, book, false);
    ready = Database_Cache::ready (bible, book);
    evaluate (__LINE__, __func__, false, ready);
    
    Database_Cache::ready (bible, book, true);
    ready = Database_Cache::ready (bible, book);
    evaluate (__LINE__, __func__, true, ready);
  }

  // Check the file size function.
  {
    string bible = "size";
    int book = 12;
    Database_Cache::create (bible, book);
    
    int size = Database_Cache::size (bible, book);
    if ((size < 10000) || (size > 15000)) {
      evaluate (__LINE__, __func__, "between 3072 and 5120", convert_to_string (size));
    }

    size = Database_Cache::size (bible, book + 1);
    evaluate (__LINE__, __func__, 0, size);
  }
  
  // Check file naming for downloading a cache.
  {
    evaluate (__LINE__, __func__, "cache_resource_", Database_Cache::fragment ());
    evaluate (__LINE__, __func__, "databases/cache_resource_download_23.sqlite", Database_Cache::path ("download", 23));
  }
  
  refresh_sandbox (true);
}


// Tests for Database_Bibles.
void test_database_bibles ()
{
  trace_unit_tests (__func__);
  
  {
    refresh_sandbox (true);
    Database_Bibles database_bibles;
    Database_State::create ();
    vector <string> standard;
    vector <string> bibles = database_bibles.getBibles ();
    evaluate (__LINE__, __func__, standard, bibles);
  }
  {
    // Test whether optimizing works without errors.
    refresh_sandbox (true);
    Database_Bibles database_bibles;
    Database_State::create ();
    database_bibles.createBible ("phpunit");
    database_bibles.storeChapter ("phpunit", 2, 3, "a");
    database_bibles.storeChapter ("phpunit", 2, 3, "b");
    database_bibles.storeChapter ("phpunit", 2, 3, "c");
    database_bibles.storeChapter ("phpunit", 2, 3, "d");
    database_bibles.storeChapter ("phpunit", 2, 3, "e");
    database_bibles.storeChapter ("phpunit", 2, 3, "f");
    database_bibles.storeChapter ("phpunit", 2, 3, "g");
    database_bibles.optimize ();
    string usfm = database_bibles.getChapter ("phpunit", 2, 3);
    evaluate (__LINE__, __func__, "g", usfm);
  }
  {
    // Test whether optimizing removes files with 0 size.
    refresh_sandbox (true);
    Database_Bibles database_bibles;
    Database_State::create ();
    database_bibles.createBible ("phpunit");
    database_bibles.storeChapter ("phpunit", 2, 3, "a");
    database_bibles.storeChapter ("phpunit", 2, 3, "b");
    database_bibles.storeChapter ("phpunit", 2, 3, "c");
    database_bibles.storeChapter ("phpunit", 2, 3, "d");
    database_bibles.storeChapter ("phpunit", 2, 3, "e");
    database_bibles.storeChapter ("phpunit", 2, 3, "f");
    database_bibles.storeChapter ("phpunit", 2, 3, "");
    string usfm = database_bibles.getChapter ("phpunit", 2, 3);
    evaluate (__LINE__, __func__, "", usfm);
    database_bibles.optimize ();
    usfm = database_bibles.getChapter ("phpunit", 2, 3);
    evaluate (__LINE__, __func__, "f", usfm);
  }
  // Test create / get / delete Bibles.
  {
    refresh_sandbox (true);
    Database_Bibles database_bibles;
    Database_State::create ();
    
    database_bibles.createBible ("phpunit");
    
    vector <string> bibles = database_bibles.getBibles ();
    evaluate (__LINE__, __func__, {"phpunit"}, bibles);
    
    database_bibles.deleteBible ("phpunit");
    
    bibles = database_bibles.getBibles ();
    evaluate (__LINE__, __func__, {}, bibles);
  }
  // Test storeChapter / getChapter
  {
    refresh_sandbox (true);
    Database_Bibles database_bibles;
    Database_State::create ();
    
    database_bibles.createBible ("phpunit");
    string usfm = "\\c 1\n\\p\n\\v 1 Verse 1";
    database_bibles.storeChapter ("phpunit", 2, 1, usfm);
    string result = database_bibles.getChapter ("phpunit", 2, 1);
    evaluate (__LINE__, __func__, usfm, result);
    result = database_bibles.getChapter ("phpunit2", 2, 1);
    evaluate (__LINE__, __func__, "", result);
    result = database_bibles.getChapter ("phpunit", 1, 1);
    evaluate (__LINE__, __func__, "", result);
  }
  // Test books
  {
    refresh_sandbox (true);
    Database_Bibles database_bibles;
    Database_State::create ();
    
    database_bibles.createBible ("phpunit");
    vector <int> books = database_bibles.getBooks ("phpunit");
    evaluate (__LINE__, __func__, { }, books);
    
    database_bibles.storeChapter ("phpunit", 1, 2, "\\c 1");
    books = database_bibles.getBooks ("phpunit");
    evaluate (__LINE__, __func__, { 1 }, books);
    
    database_bibles.storeChapter ("phpunit", 2, 3, "\\c 0");
    books = database_bibles.getBooks ("phpunit");
    evaluate (__LINE__, __func__, { 1, 2 }, books);
    
    database_bibles.deleteBook ("phpunit", 3);
    books = database_bibles.getBooks ("phpunit");
    evaluate (__LINE__, __func__, { 1, 2 }, books);
    
    database_bibles.deleteBook ("phpunit", 1);
    books = database_bibles.getBooks ("phpunit");
    evaluate (__LINE__, __func__, { 2 }, books);
    
    database_bibles.deleteBook ("phpunit2", 2);
    books = database_bibles.getBooks ("phpunit");
    evaluate (__LINE__, __func__, { 2 }, books);
  }
  // Test chapters ()
  {
    refresh_sandbox (true);
    Database_Bibles database_bibles;
    Database_State::create ();
    
    database_bibles.createBible ("phpunit");
    vector <int> chapters = database_bibles.getChapters ("phpunit", 1);
    evaluate (__LINE__, __func__, { }, chapters);
    
    database_bibles.storeChapter ("phpunit", 1, 2, "\\c 1");
    chapters = database_bibles.getChapters ("phpunit", 1);
    evaluate (__LINE__, __func__, { 2 }, chapters);
    
    chapters = database_bibles.getChapters ("phpunit", 2);
    evaluate (__LINE__, __func__, { }, chapters);
    
    database_bibles.storeChapter ("phpunit", 1, 3, "\\c 1");
    chapters = database_bibles.getChapters ("phpunit", 1);
    evaluate (__LINE__, __func__, { 2, 3 }, chapters);
    
    database_bibles.deleteChapter ("phpunit", 3, 3);
    chapters = database_bibles.getChapters ("phpunit", 1);
    evaluate (__LINE__, __func__, { 2, 3 }, chapters);
    
    database_bibles.deleteChapter ("phpunit", 1, 2);
    chapters = database_bibles.getChapters ("phpunit", 1);
    evaluate (__LINE__, __func__, { 3 }, chapters);
    
    database_bibles.deleteChapter ("phpunit", 1, 3);
    chapters = database_bibles.getChapters ("phpunit", 1);
    evaluate (__LINE__, __func__, { }, chapters);
  }
  // Test chapter IDs
  {
    refresh_sandbox (true);
    Database_Bibles database_bibles;
    Database_State::create ();
    
    database_bibles.createBible ("phpunit");
    database_bibles.storeChapter ("phpunit", 1, 2, "\\c 1");
    int id = database_bibles.getChapterId ("phpunit", 1, 2);
    evaluate (__LINE__, __func__, 100000001, id);
    
    database_bibles.storeChapter ("phpunit", 1, 2, "\\c 1");
    id = database_bibles.getChapterId ("phpunit", 1, 2);
    evaluate (__LINE__, __func__, 100000002, id);
    
    database_bibles.storeChapter ("phpunit", 1, 2, "\\c 1");
    database_bibles.storeChapter ("phpunit", 1, 2, "\\c 1");
    id = database_bibles.getChapterId ("phpunit", 1, 2);
    evaluate (__LINE__, __func__, 100000004, id);
    
    database_bibles.storeChapter ("phpunit", 2, 3, "\\c 1");
    id = database_bibles.getChapterId ("phpunit", 1, 2);
    evaluate (__LINE__, __func__, 100000004, id);
  }
}


void test_database_login ()
{
  trace_unit_tests (__func__);

  {
    refresh_sandbox (true);
    Database_Login::create ();
    string path = database_sqlite_file (Database_Login::database ());
    filter_url_file_put_contents (path, "damaged database");
    evaluate (__LINE__, __func__, false, Database_Login::healthy ());
    Database_Login::optimize ();
    evaluate (__LINE__, __func__, true, Database_Login::healthy ());
    refresh_sandbox (false);
  }
  
  refresh_sandbox (true);
  Database_Login::create ();
  Database_Login::optimize ();
  
  string username = "unittest";
  string username2 = "unittest2";
  string address = "192.168.1.0";
  string agent = "Browser's user agent";
  string fingerprint = "ԴԵԶԸ";
  string cookie = "abcdefghijklmnopqrstuvwxyz";
  string cookie2 = "abcdefghijklmnopqrstuvwxyzabc";
  bool daily;

  // Testing whether setting tokens and reading the username, and removing the tokens works.
  Database_Login::setTokens (username, address, agent, fingerprint, cookie, true);
  evaluate (__LINE__, __func__, username, Database_Login::getUsername (cookie, daily));
  Database_Login::removeTokens (username);
  evaluate (__LINE__, __func__, "", Database_Login::getUsername (cookie, daily));

  // Testing whether a persistent login gets removed after about a year.
  Database_Login::setTokens (username, address, agent, fingerprint, cookie, true);
  evaluate (__LINE__, __func__, username, Database_Login::getUsername (cookie, daily));
  Database_Login::testTimestamp ();
  Database_Login::trim ();
  evaluate (__LINE__, __func__, "", Database_Login::getUsername (cookie, daily));

  // Testing whether storing touch enabled
  Database_Login::setTokens (username, address, agent, fingerprint, cookie, true);
  evaluate (__LINE__, __func__, true, Database_Login::getTouchEnabled (cookie));
  Database_Login::removeTokens (username);
  evaluate (__LINE__, __func__, false, Database_Login::getTouchEnabled (cookie));
  Database_Login::setTokens (username, address, agent, fingerprint, cookie, true);
  evaluate (__LINE__, __func__, false, Database_Login::getTouchEnabled (cookie + "x"));

  // Testing that removing tokens for one set does not remove all tokens for a user.
  Database_Login::setTokens (username, address, agent, fingerprint, cookie, true);
  evaluate (__LINE__, __func__, username, Database_Login::getUsername (cookie, daily));
  Database_Login::setTokens (username, address, agent, fingerprint, cookie2, true);
  evaluate (__LINE__, __func__, username, Database_Login::getUsername (cookie2, daily));
  Database_Login::removeTokens (username, cookie2);
  evaluate (__LINE__, __func__, username, Database_Login::getUsername (cookie, daily));
  evaluate (__LINE__, __func__, "", Database_Login::getUsername (cookie2, daily));
  
  // Test moving tokens to a new username.
  Database_Login::removeTokens (username);
  Database_Login::setTokens (username, address, agent, fingerprint, cookie, true);
  evaluate (__LINE__, __func__, username, Database_Login::getUsername (cookie, daily));
  Database_Login::renameTokens (username, username2, cookie);
  evaluate (__LINE__, __func__, username2, Database_Login::getUsername (cookie, daily));
}


void test_database_privileges ()
{
  trace_unit_tests (__func__);

  // Test creation, automatic repair of damages.
  refresh_sandbox (true);
  Database_Privileges::create ();
  string path = database_sqlite_file (Database_Privileges::database ());
  filter_url_file_put_contents (path, "damaged database");
  evaluate (__LINE__, __func__, false, Database_Privileges::healthy ());
  Database_Privileges::optimize ();
  evaluate (__LINE__, __func__, true, Database_Privileges::healthy ());
  refresh_sandbox (false);
  
  Database_Privileges::create ();
  
  // Upgrade routine should not give errors.
  Database_Privileges::upgrade ();
  
  string username = "phpunit";
  string bible = "bible";
  
  // Initially there's no privileges for a Bible book.
  bool read;
  bool write;
  Database_Privileges::getBibleBook (username, bible, 2, read, write);
  evaluate (__LINE__, __func__, false, read);
  evaluate (__LINE__, __func__, false, write);

  // Set privileges and read them.
  Database_Privileges::setBibleBook (username, bible, 3, false);
  Database_Privileges::getBibleBook (username, bible, 3, read, write);
  evaluate (__LINE__, __func__, true, read);
  evaluate (__LINE__, __func__, false, write);
  
  Database_Privileges::setBibleBook (username, bible, 4, true);
  Database_Privileges::getBibleBook (username, bible, 4, read, write);
  evaluate (__LINE__, __func__, true, read);
  evaluate (__LINE__, __func__, true, write);
  
  Database_Privileges::removeBibleBook (username, bible, 4);
  Database_Privileges::getBibleBook (username, bible, 4, read, write);
  evaluate (__LINE__, __func__, false, read);
  evaluate (__LINE__, __func__, false, write);

  // Test Bible book entry count.
  Database_Privileges::setBibleBook (username, bible, 6, true);
  int count = Database_Privileges::getBibleBookCount ();
  evaluate (__LINE__, __func__, 2, count);
  
  // Test removing book zero, that it removes entries for all books.
  Database_Privileges::removeBibleBook (username, bible, 0);
  count = Database_Privileges::getBibleBookCount ();
  evaluate (__LINE__, __func__, 0, count);
  
  // Enter a privilege for book = 1, and a different privilege for book 0,
  // and then test that the privilege for book 1 has preference.
  Database_Privileges::setBibleBook (username, bible, 1, false);
  Database_Privileges::getBibleBook (username, bible, 1, read, write);
  evaluate (__LINE__, __func__, true, read);
  evaluate (__LINE__, __func__, false, write);
  Database_Privileges::setBibleBook (username, bible, 0, true);
  Database_Privileges::setBibleBook (username, bible, 1, false);
  Database_Privileges::getBibleBook (username, bible, 1, read, write);
  evaluate (__LINE__, __func__, true, read);
  evaluate (__LINE__, __func__, false, write);

  // Start afresh to not depend too much on previous tests.
  refresh_sandbox (true);
  Database_Privileges::create ();
  
  // Test whether an entry for a book exists.
  bool exists = Database_Privileges::getBibleBookExists (username, bible, 0);
  evaluate (__LINE__, __func__, false, exists);
  Database_Privileges::setBibleBook (username, bible, 1, false);
  // Test the record for the correct book.
  exists = Database_Privileges::getBibleBookExists (username, bible, 1);
  evaluate (__LINE__, __func__, true, exists);
  // The record should also exist for book 0.
  exists = Database_Privileges::getBibleBookExists (username, bible, 0);
  evaluate (__LINE__, __func__, true, exists);
  // The record should not exist for another book.
  exists = Database_Privileges::getBibleBookExists (username, bible, 2);
  evaluate (__LINE__, __func__, false, exists);

  // Start afresh to not depend on the outcome of previous tests.
  refresh_sandbox (true);
  Database_Privileges::create ();

  // Test no read access to entire Bible.
  Database_Privileges::getBible (username, bible, read, write);
  evaluate (__LINE__, __func__, false, read);
  evaluate (__LINE__, __func__, false, write);
  // Set Bible read-only and test it.
  Database_Privileges::setBible (username, bible, false);
  Database_Privileges::getBible (username, bible, read, write);
  evaluate (__LINE__, __func__, true, read);
  evaluate (__LINE__, __func__, false, write);
  // Set Bible read-write, and test it.
  Database_Privileges::setBible (username, bible, true);
  Database_Privileges::getBible (username, bible, read, write);
  evaluate (__LINE__, __func__, true, read);
  evaluate (__LINE__, __func__, true, write);
  // Set one book read-write and test that this applies to entire Bible.
  Database_Privileges::setBible (username, bible, false);
  Database_Privileges::setBibleBook (username, bible, 1, true);
  Database_Privileges::getBible (username, bible, read, write);
  evaluate (__LINE__, __func__, true, read);
  evaluate (__LINE__, __func__, true, write);
  
  // A feature is off by default.
  bool enabled = Database_Privileges::getFeature (username, 123);
  evaluate (__LINE__, __func__, false, enabled);

  // Test setting a feature on and off.
  Database_Privileges::setFeature (username, 1234, true);
  enabled = Database_Privileges::getFeature (username, 1234);
  evaluate (__LINE__, __func__, true, enabled);
  Database_Privileges::setFeature (username, 1234, false);
  enabled = Database_Privileges::getFeature (username, 1234);
  evaluate (__LINE__, __func__, false, enabled);
  
  // Test bulk privileges removal.
  refresh_sandbox (true);
  Database_Privileges::create ();
  // Set privileges for user for Bible.
  Database_Privileges::setBible (username, bible, false);
  count = Database_Privileges::getBibleBookCount ();
  evaluate (__LINE__, __func__, 1, count);
  // Remove privileges for a Bible and check on them.
  Database_Privileges::removeBible (bible);
  count = Database_Privileges::getBibleBookCount ();
  evaluate (__LINE__, __func__, 0, count);
  // Again, set privileges, and remove them by username.
  Database_Privileges::setBible (username, bible, false);
  count = Database_Privileges::getBibleBookCount ();
  evaluate (__LINE__, __func__, 1, count);
  Database_Privileges::removeUser (username);
  count = Database_Privileges::getBibleBookCount ();
  evaluate (__LINE__, __func__, 0, count);
  
  // Set features for user.
  Database_Privileges::setFeature (username, 1234, true);
  enabled = Database_Privileges::getFeature (username, 1234);
  evaluate (__LINE__, __func__, true, enabled);
  // Remove features by username.
  Database_Privileges::removeUser (username);
  enabled = Database_Privileges::getFeature (username, 1234);
  evaluate (__LINE__, __func__, false, enabled);
  
  // Test privileges transfer through a text file.
  refresh_sandbox (true);
  Database_Privileges::create ();
  // Set privileges.
  Database_Privileges::setBibleBook (username, bible, 1, true);
  Database_Privileges::setFeature (username, 1234, true);
  // Check the transfer text file.
  string privileges =
    "bibles_start\n"
    "bible\n"
    "1\n"
    "on\n"
    "bibles_end\n"
    "features_start\n"
    "1234\n"
    "features_start";
  evaluate (__LINE__, __func__, privileges, Database_Privileges::save (username));
  // Transfer the privileges to another user.
  string clientuser = username + "client";
  Database_Privileges::load (clientuser, privileges);
  // Check the privileges for that other user.
  Database_Privileges::getBible (clientuser, bible, read, write);
  evaluate (__LINE__, __func__, true, read);
  evaluate (__LINE__, __func__, true, write);
  enabled = Database_Privileges::getFeature (username, 1234);
  evaluate (__LINE__, __func__, true, enabled);
}


void test_database_git ()
{
#ifdef HAVE_CLOUD
  trace_unit_tests (__func__);
  
  refresh_sandbox (true);
  
  // Create the database.
  Database_Git::create ();

  string user = "user";
  string bible = "bible";
  
  // Store one chapter, and check there's one rowid as a result.
  Database_Git::store_chapter (user, bible, 1, 2, "old", "new");
  vector <int> rowids = Database_Git::get_rowids (user, "");
  evaluate (__LINE__, __func__, {}, rowids);
  rowids = Database_Git::get_rowids ("", bible);
  evaluate (__LINE__, __func__, {}, rowids);
  rowids = Database_Git::get_rowids (user, bible);
  evaluate (__LINE__, __func__, {1}, rowids);

  // Store some more chapters to get more rowids in the database.
  Database_Git::store_chapter (user, bible, 2, 5, "old2", "new5");
  Database_Git::store_chapter (user, bible, 3, 6, "old3", "new6");
  Database_Git::store_chapter (user, bible, 4, 7, "old4", "new7");

  // Retrieve and check a certain rowid whether it has the correct values.
  string user2, bible2;
  int book, chapter;
  string oldusfm, newusfm;
  bool get = Database_Git::get_chapter (1, user2, bible2, book, chapter, oldusfm, newusfm);
  evaluate (__LINE__, __func__, true, get);
  evaluate (__LINE__, __func__, user, user2);
  evaluate (__LINE__, __func__, bible, bible2);
  evaluate (__LINE__, __func__, 1, book);
  evaluate (__LINE__, __func__, 2, chapter);
  evaluate (__LINE__, __func__, "old", oldusfm);
  evaluate (__LINE__, __func__, "new", newusfm);
  
  // Erase a rowid, and check that the remaining ones in the database are correct.
  Database_Git::erase_rowid (2);
  rowids = Database_Git::get_rowids (user, bible);
  evaluate (__LINE__, __func__, {1, 3, 4}, rowids);

  // Getting a non-existent rowid should fail.
  get = Database_Git::get_chapter (2, user, bible, book, chapter, oldusfm, newusfm);
  evaluate (__LINE__, __func__, false, get);
  
  // Update the timestamps and check that expired entries get removed and recent ones remain.
  rowids = Database_Git::get_rowids ("user", "bible");
  evaluate (__LINE__, __func__, 3, rowids.size ());
  Database_Git::optimize ();
  rowids = Database_Git::get_rowids (user, bible);
  evaluate (__LINE__, __func__, 3, rowids.size ());
  Database_Git::touch_timestamps (filter_date_seconds_since_epoch () - 432000 - 1);
  Database_Git::optimize ();
  rowids = Database_Git::get_rowids (user, bible);
  evaluate (__LINE__, __func__, 0, rowids.size ());
  
  // Test that it reads distinct users.
  Database_Git::store_chapter (user, bible, 2, 5, "old", "new");
  Database_Git::store_chapter (user, bible, 2, 5, "old", "new");
  Database_Git::store_chapter ("user2", bible, 2, 5, "old", "new");
  vector <string> users = Database_Git::get_users (bible);
  evaluate (__LINE__, __func__, {user, "user2"}, users);
#endif
}


void test_database_userresources ()
{
  trace_unit_tests (__func__);
  
  refresh_sandbox (true);
  
  vector <string> names;
  string name = "unit//test";
  string url = "https://website.org/resources/<book>/<chapter>/<verse>.html";
  int book = 99;
  string abbrev = "Book 99";
  string fragment;
  string value;
  vector <string> specialnames = { "abc\\def:ghi", name };
  
  names = Database_UserResources::names ();
  evaluate (__LINE__, __func__, {}, names);

  Database_UserResources::url (name, url);
  value = Database_UserResources::url (name);
  evaluate (__LINE__, __func__, url, value);
  
  for (auto name : specialnames) {
    Database_UserResources::url (name, name + url);
  }
  names = Database_UserResources::names ();
  evaluate (__LINE__, __func__, specialnames, names);

  for (auto name : specialnames) {
    Database_UserResources::remove (name);
  }
  names = Database_UserResources::names ();
  evaluate (__LINE__, __func__, {}, names);

  Database_UserResources::book (name, book, abbrev);
  fragment = Database_UserResources::book (name, book);
  evaluate (__LINE__, __func__, abbrev, fragment);

  fragment = Database_UserResources::book (name + "x", book);
  evaluate (__LINE__, __func__, "", fragment);

  fragment = Database_UserResources::book (name, book + 1);
  evaluate (__LINE__, __func__, "", fragment);
}


void test_database_statistics ()
{
#ifdef HAVE_CLOUD
  trace_unit_tests (__func__);
  
  {
    refresh_sandbox (true);
    
    Database_Statistics::create ();
    Database_Statistics::optimize ();
    
    int one_thousand = 1000;
    int two_thousand = 2000;
    int now = filter_date_seconds_since_epoch ();
    int now_plus_one = now + 1;
    
    // Record some statistics.
    Database_Statistics::store_changes (one_thousand, "zero", 0);
    Database_Statistics::store_changes (one_thousand, "one", 10);
    Database_Statistics::store_changes (two_thousand, "one", 20);
    Database_Statistics::store_changes (now, "one", 30);
    Database_Statistics::store_changes (now_plus_one, "one", 40);
    Database_Statistics::store_changes (1100, "two", 11);
    Database_Statistics::store_changes (2100, "two", 21);
    Database_Statistics::store_changes (now - one_thousand, "two", 31);
    Database_Statistics::store_changes (now - two_thousand, "two", 41);

    // Check all available users.
    vector <string> users = Database_Statistics::get_users ();
    evaluate (__LINE__, __func__, {"one", "two"}, users);

    // The changes for all available users for no more than a year ago.
    vector <pair <int, int>> changes = Database_Statistics::get_changes ("");
    evaluate (__LINE__, __func__, 4, changes.size ());

    // A known amount of change statistics records for a known user for no more than a year ago.
    changes = Database_Statistics::get_changes ("two");
    unsigned int size = 2;
    evaluate (__LINE__, __func__, size, changes.size ());
    
    // Sort the change statistics most recent first.
    if (changes.size () == size) {
      evaluate (__LINE__, __func__, now - one_thousand, changes[0].first);
      evaluate (__LINE__, __func__, 31, changes[0].second);
      evaluate (__LINE__, __func__, now - two_thousand, changes[1].first);
      evaluate (__LINE__, __func__, 41, changes[1].second);
    }
  }
#endif
}


void test_database_develop ()
{
  trace_unit_tests (__func__);
}
