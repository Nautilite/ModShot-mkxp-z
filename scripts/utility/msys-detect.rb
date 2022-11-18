# This Ruby script that determines current MSYS2 arch
# and outputs prefix for libraries.

case ENV["MSYSTEM"].downcase
when 'mingw64'
  puts 'x64-msvcrt'
when 'mingw32'
  puts 'msvcrt'
when 'ucrt64', 'clang64', 'clangarm64'
  puts 'x64-ucrt'
when 'clang32'
  puts 'ucrt'
end
