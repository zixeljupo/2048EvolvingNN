	В рамках разрабатываемого проекта необходимо написать бота для самостоятельной игры в 2048. Желательная цель - достижение большого счета в игре. Минимальная цель - сделать бота, который способен получать ячейку с числом 4096.
Ниже приведен один из нескольких возможных вариантов реализации бота, которого я буду придерживаться в дальнейшем.

Бот состоит из следующих основных частей:
-
+ 1.	Искусственная нейронная сеть с возможностью прямого распространения. Нейронная сеть - некоторая непрерывная функция, принимающая на вход вектор и возвращающая вектор. Вывод сети зависит от большого количества внутренних параметров. В данной задаче нейронная сеть является функцией оценки игрового состояния, т.е. на выходе у нее одно число - безразмерная оценка позиции.
+ 2.	Генетический алгоритм. Множество внутренних параметров сети определяет качество бота. Для получения этих неизвестных параметров используется генетический алгоритм. Геном нейронной сети (особи) - вектор ее внутренних параметров. Генетический алгоритм, обучающий нейронную сеть, работает следующим образом. При инициализации алгоритма создается начальная популяция нейронных сетей, параметры которых выбираются случайно. После чего алгоритм итеративно выполняет следующие действия:
  + 1) оценка качества каждой особи;
  + 2) отбор лучших особей популяции;
  + 3) удаление остальных особей популяции;
  + 4) создание необходимого количества особей для получения первоначальной популяции.
- Качество игры особи оценивается по множеству игр данной особи. Среди всех игр выбирается медиана, которая представляет итоговую оценку качества особи. Чем лучше нейросеть играет, тем выше будет медианное значение по множеству игр.
Отбор лучших особей популяции заключается в сортировке особей по их качеству, после чего константная часть этих сетей отбрасывается.
Каждая новая особь создается следующим образом. Выбираются две особи среди лучших особей, между геномами которых происходит кроссинговер (обмен параметрами), в результате которого получается новая особь, наследующая некоторые свойства своих родителей. После чего к данной особи применяются различные мутации, изменяющие случайным образом ее геном.

+ 3.	Последняя важная часть бота - поиск оптимальных ходов перебором позиций. Предположительно, будет использоваться поиск в ширину на фиксированную глубину. При этом каждая из получившихся позиций на последнем уровне глубины будет оцениваться нейронной сетью. Выбор хода будет осуществляться так, чтобы попасть в позицию с максимальным счетом на последнем уровне глубины.

Дополнительные сведения:
-
На данный момент мною проделана большая часть работы. В частности, написана реализация нейронной сети и генетического алгоритма. После этого при помощи профилировщика было обнаружено, что дольше всего при обучении нейронных сетей выполняется прямое распространение сигнала по сети. После чего код был частично переписан так, чтобы ассемблерный код был максимально быстрым. В частности, было указано несколько параметров компиляции, при которых прямое распространение в нейронной сети происходит максимально эффективно. Неполный список оптимизирующих настроек: установлен режим fp:fast вместо fp:precise, позволяющий пользоваться перестановочностью математических операций над числами с плавающей точкой (х1.5 ускорение всей программы), включен флаг оптимизации Ox вместо O0 (что принесло ~х40 к производительности), была включена аппаратная поддержка AVX (ускорение не замерялось), включены подстановки всех функций, которые было возможно подставить. Также я узнал, что в Visual Studio Ctrl-F5 запускает Debug версию вне режима отладки, хотя я думал, что это запускает Release версию. Запустив Release, я снова увидел рост производительности в несколько раз. Такие дела.

Также для обучения используется несколько потоков выполнения. Изначально их было 8, что загружало процессор на 100% CPU. (8 под количество логических ядер) Потом выяснилось, что если запускать в 4 потока, под число ядер, то процессор загружается не полностью, но эпоха обучения проходит примерно в 1.5 раза быстрее. Это легко объясняется тем, что производительность всей системы зависит на 99% от одной единственной операции - скалярного умножения векторов достаточно большого размера. А это значит, что итоговая производительность ограничена лишь числом FLOPS, и не зависит от количества логических процессоров.