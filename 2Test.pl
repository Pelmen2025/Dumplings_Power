% Функция для вычисления ближайших степеней двойки
next_power_of_two(X, P) :- 
    P is 2 ** (floor(log(X) / log(2)) + 1).

previous_power_of_two(X, P) :- 
    P is 2 ** floor(log(X) / log(2)).

% Предикат для чтения списка целых чисел с клавиатуры
read_integer_list(List) :-
    write('Введите элементы списка через пробел: '),
    read_line_to_string(user_input, Input),
    split_string(Input, " ", "", StringList),
    maplist(atom_number, StringList, List).

% Основная логика для нахождения всех положительных элементов списка
find_between_powers_of_two :-
    read_integer_list(List),
    include(positive, List, PositiveList),
    (PositiveList == [] -> 
        write('Нет положительных чисел в списке.'), nl
    ;   
        min_list(PositiveList, Min),
        max_list(PositiveList, Max),
        
        LowerBound is floor(log(Min) / log(2)),  
        UpperBound is floor(log(Max) / log(2)) + 1,
        
        format('Все положительные числа расположены между степенями двойки: 2^~w и 2^~w.~n', 
               [LowerBound, UpperBound])
    ).

% Проверка на положительное число
positive(X) :-
    X > 0.

% Запуск программы
:- find_between_powers_of_two.
