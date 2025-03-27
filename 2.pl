% Предикат для чтения списка целых чисел с клавиатуры
read_integer_list(List) :-
    write('Введите положительные числа списка через пробел: '),
    read_line_to_string(user_input, Input),
    split_string(Input, " ", "", StringList),
    maplist(atom_number, StringList, List).

% Основная логика для нахождения диапазона степеней двойки
find_between_powers_of_two :-
    read_integer_list(List),
    List \= [], % Проверяем, что список не пустой
    min_list(List, Min),
    max_list(List, Max),
    
    LowerBound is floor(log(Min) / log(2)),  
    UpperBound is floor(log(Max) / log(2)) + 1,
    
    format('Все числа расположены между степенями двойки: 2^~w и 2^~w.~n', 
           [LowerBound, UpperBound]).

% Запуск программы
:- find_between_powers_of_two.
