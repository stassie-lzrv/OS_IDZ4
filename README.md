# ОС ИДЗ №4
## Лазарева Анастасия БПИ-217
### Вариант 28: Задача о Винни-Пухе – 3 или мстительные пчелы. Неправильные пчелы, подсчитав в конце месяца убытки от наличия в лесу Винни-Пуха, решили разыскать его и наказать в назидание всем другим любителям сладкого. Для поисков медведя они поделили лес на участки, каждый из которых прочесывает одна стая неправильных пчел. В случае нахождения медведя на своем участке стая проводит показательное наказание и возвращается в улей. Если участок прочесан, а Винни-Пух на нем не обнаружен, стая также возвращается в улей. Там она получает информацию об еще неисследованных участках и снова улетает. Требуется создать приложение, моделирующее действия пчел. Каждая стая — отдельный клиент. Сервер — улей. Время обследования участка задается случайно.

#### ИДЗ было выполнено согласно критериям на оценку 8
## 4-5 баллов

Разработать клиент–серверное приложение, в котором сервер (или серверы) и клиенты независимо друг от друга отображают только ту информацию, которая поступает им во время обмена. То есть, отсутствует какой-либо общий вывод интегрированной информации, отображающий поведение системы в целом.

При запуске программ требуемые для их работы IP адреса и порты необходимо задавать в командной строке, чтобы обеспечить гибкую подстройку к любой сети.

Разработанное приложение должно работать как на одном компьютере так и в распределенном (сетевом) режиме на нескольких компьютерах, по которым можно будет разнести серверы и клиентов.

#### Как запустить?
gcc bee.c -o bee

gcc hive.c -o hive

./hive 8000 <кол-во секторов> <номер сектора, где находится Винни-Пух>

./bee 127.0.0.1 8000 (можно несколько раз)

### Результаты тестирования
![img](/4-5/image1.png)

![img](/4-5/image2.png)

## 6-8 баллов
В дополнение к программе на предыдущую оценку необходимо разработать клиентскую программу, подключаемую к серверу, которая предназначена для отображение комплексной информации о выполнении приложения в целом. То есть, данный программный модуль должен адекватно отображать поведение моделируемой системы, позволяя не пользоваться отдельными видами, предоставляемыми клиентами и серверами по отдельности. В дополнение к предыдущей программе реализовать возможность, подключения множества клиентов, обеспечивающих отображение информации о работе приложения. Это должно позволить осуществлять наблюдение за поведением программы с многих независимых компьютеров. При этом клиентов–наблюдателей можно отключать и подключать снова в динамическом режиме без нарушения работы всего приложения.

Для внешней программы используется мультикаст.

#### Как запустить?
gcc bee.c -o bee

gcc hive.c -o hive

gcc obs.c - o obs

./hive 224.0.0.1 8001 8000 <кол-во секторов> <номер сектора, где находится Винни-Пух>

./obs 224.0.0.1 8001

./bee 127.0.0.1 8000 (можно несколько раз)


### Результаты тестирования
![img](/6-8/image4.png)

![img](/6-8/image5.png)

![img](/6-8/image6.png)

![img](/6-8/image7.png)

