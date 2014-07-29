// -*- coding: iso-8859-1-unix -*-

#include <openbus/assistant.hpp>
#include <openbus/assistant/waitlogin.hpp>

int main(int argc, char** argv)
{
  typedef openbus::assistant::Assistant::login_error_callback_type login_error_callback_type;
  typedef openbus::assistant::Assistant::register_error_callback_type register_error_callback_type;
  typedef openbus::assistant::Assistant::fatal_error_callback_type fatal_error_callback_type;
  typedef openbus::assistant::Assistant::find_error_callback_type find_error_callback_type;

  openbus::assistant::Assistant assistant
    = openbus::assistant::Assistant::createWithPassword
    ("demo", "demo", "localhost", 2089, argc, argv
     , login_error_callback_type()
     , register_error_callback_type()
     , fatal_error_callback_type()
     , find_error_callback_type()
     , logger::debug_level);

  waitLogin(assistant);
  assistant.shutdown();
}
