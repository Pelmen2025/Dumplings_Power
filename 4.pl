start :-
    write('Решение логической задачи о детях и их животных.'), nl,
    write('Известно, что:'), nl,
    write('1. Петя не любит кошек и живет в одном подъезде с хозяйкой хомячка.'), nl,
    write('2. Лена дружит с Таней, гуляющей с кошкой.'), nl, nl,
    write('Нажмите Enter чтобы увидеть решение...'), nl,
    read_line(_),
    solve_problem.

solve_problem :-
    % Определяем возможных животных
    Animals = [dog, cat, hamster],
    
    % Генерируем все возможные распределения животных
    member(AnimalPetya, Animals),
    member(AnimalLena, Animals),
    member(AnimalTanya, Animals),
    all_different([AnimalPetya, AnimalLena, AnimalTanya]),
    
    % Проверяем условия задачи
    % Условие 1: Петя не любит кошек (у него не cat)
    AnimalPetya \= cat,
    
    % Условие 2: Петя живет в одном подъезде с хозяйкой хомячка
    % (значит, хозяйка хомячка - не Петя, так как он не может быть сам с собой)
    (AnimalLena = hamster ; AnimalTanya = hamster),
    AnimalPetya \= hamster,
    
    % Условие 3: Лена дружит с Таней, гуляющей с кошкой
    AnimalTanya = cat,
    
    % Выводим решение
    write('Решение:'), nl,
    format('Петя гуляет с ~w', [AnimalPetya]), nl,
    format('Лена гуляет с ~w', [AnimalLena]), nl,
    format('Таня гуляет с ~w', [AnimalTanya]), nl, nl,
    write('Нажмите Enter чтобы завершить...'), nl,
    read_line(_).

% Предикат для проверки, что все элементы списка разные
all_different([]).
all_different([H|T]) :-
    \+ member(H, T),
    all_different(T).

% Чтение строки с клавиатуры
read_line(Line) :-
    get_char(C),
    ( C = '\n' -> Line = []
    ; Line = [C|Rest], read_line(Rest)
    ).

% Запуск программы
:- initialization(start).
