QImage和QPixMap都是Qt中用于表示图像的类，它们的区别主要在于：

QImage和QPixMap底层数据的存储方式不同：QImage的像素数据存储在内存中，而QPixMap则是使用显存来存储像素数据。因此，当需要处理大量图像数据时，QImage会占用更多的内存，而QPixMap的处理速度更快，因为它直接使用图形硬件。
QImage和QPixMap的应用场景不同：QImage可以在内存中进行高级图像处理操作，例如将一个图像缩放或者应用滤镜效果，然后将处理后的图像渲染到QPixMap或者QWidget上显示。而QPixMap通常用于直接在窗口或者其他QWidget上绘制位图，例如游戏动画或者用户界面元素。
QImage和QPixMap对透明度的处理方式不同：QPixMap中的像素不支持透明度，如果需要使用透明度，需要将QPixMap和QPainter的compositing模式一起使用。而QImage可以支持不同的透明度设置，对于图像处理上涉及到透明通道的操作，比如将一些图像合并、叠加，会更加便捷。

综上所述，QImage和QPixMap在应用场景、像素数据存储方式、透明度处理等方面具有不同的特点。根据实际需求，可以选择合适的类来处理图像。


## qt中connect的第五个参数，是干什么用的？

在Qt中，connect()函数用于连接一个信号(signal)和槽(slot)。connect()函数的第五个参数是用于实现一个队列(Queued Connection)或立即调用(Direct Connection)方式连接的标志。

connect()函数的完整形式如下：

connect(sender, &Sender::signal, receiver, &Receiver::slot, Qt::ConnectionType connectionType)
其中，connectionType参数用来声明信号和槽之间的连接方式。connectionType参数既可以是Qt::ConnectionType枚举类型中的常量，也可以是直接使用它们的整数值。常见的常量值有：

Qt::AutoConnection: 如果信号和槽在同一个线程中，则使用Qt::DirectConnection，否则使用Qt::QueuedConnection。
Qt::DirectConnection: 当信号发射时，槽函数会立即执行，不管它们在哪个线程中。
Qt::QueuedConnection: 当信号发射时，槽函数会被添加到接收者对象所在的线程的事件队列中，并在接收者线程的事件循环中执行。
第五个参数的默认值是Qt::AutoConnection。使用Qt::DirectConnection连接方式可以快速地执行信号和槽之间的连接，但是如果信号发射和槽函数的执行在不同的线程中，那么这种方式可能会导致问题，因此使用Qt::QueuedConnection连接方式更加安全。


Qt支持6种连接方式，其中前3种最主要:
1.Qt::AutoConnection（自动方式）
Qt的默认连接方式，如果信号的发出和接收这个信号的对象同属一个线程，那个工作方式与直连方式相同；否则工作方式与排队方式相同。
2.Qt::DirectConnection（直接连接）（同步）
当信号发送后，相应的槽函数将立即被调用。emit语句后的代码将在所有槽函数执行完毕后被执行。
3.Qt::QueuedConnection（排队连接）（异步）
当信号发出后，排队到信号队列中，需等到接收对象所属线程的事件循环取得控制权时才取得该信号，调用相应的槽函数。emit语句后的代码将在发出信号后立即被执行，无需等待槽函数执行完毕。多线程环境下可使用。
4.Qt::BlockingQueuedConnection(阻塞连接，信号和槽必须在不同的线程中，否则就产生死锁)
发送信号后发送者所在的线程会处于阻塞状态 ，直到槽函数运行完。多线程同步环境下可使用。
5.Qt::UniqueConnection
与默认工作方式相同，只是不能重复连接相同的信号和槽，因为如果重复连接就会导致一个信号发出，对应槽函数就会执行多次。这个flag可以通过按位或（|）与以上四个结合在一起使用
6.Qt::AutoCompatConnection
是为了连接Qt4与Qt3的信号槽机制兼容方式，工作方式与Qt::AutoConnection一样。

QListWidget是基于Item，而QListView是基于Model的。


QLineEdit控件textChanged信号和textEdited信号区别：

textChanged信号：每当文本改变时，就会发出这个信号。文本参数是新文本。

与textEdited()不同，当以编程方式更改文本时，例如，通过调用setText()也会发出此信号。

 

textEdited信号：每当文本被编辑时，就会发出这个信号。文本参数是新文本。

与textChanged()不同，当以编程方式更改文本时，例如，通过调用setText()不会发出此信号。

Alt+0     显示或者隐藏侧边条，编辑模式下起作用（有时写的函数太长，屏幕不够大，就用这个）

Ctrl+Space  自动补全（貌似会和输入法的切换冲突）

ESc     切换到编辑模式

- 修改变量名,并应用到所有使用该变量的地方。

将光标移动到需要更改的变量上,按Ctrl + Shift + R,当前变量名称外框为红色时,表示已经已激活全局修改功能,当修改此处变量名称时将一同修改代码中所有使用该变量的变量名。

- 书签功能

Qt Creator中有一个叫做书签功能,即在某行代码处进行标记,方便以后找到。书签也可以添加文字标注。Qt中

按Ctrl + M 添加/删除书签,

按Ctrl + . 查找并移动到下一个标签

- 分栏显示

这个功能只要用 Qt Creator开发基本上都会用到。这个快捷键操作方法比较特别:

先按Ctrl + e后松开再按2添加上下布局的分栏

先按Ctrl + e后松开再按3添加上下布局的分栏

先按Ctrl + e后松开再按1删除所有的分栏



Ctrl + shift + up 将当前行的代码向上移动一行。

Ctrl + shift + down 将当前行的代码向下移动一行。

QPixmap主要用于绘图，针对图像显示而最佳化设计；

QImage主要是为图像I/O、图像访问及像素修改而设计。但若用QPixmap加载大图片的时候，会占用很大内存，适合加载小图片；一个几十K的图片，加载进来后会放大很多倍。

若图片过大，可用QImage进行加载，然后转成QPixmap用户绘制，QPixmap绘制效果最好。

QListWidget是继承QListView，QListView是基于Model的，需要自己来建模（如建立QStringListModel，QSqlTableModel等），保存数据，这样就大大降低了数据冗余，提高了程序的效率，但是需要我们对数据建模有一定的了解，而QListWidget是一个升级版本的QListView，它已经为我们建立了一个数据存储模型QListWidgetItem，操作方便，直接调用addItem即可添加Item项。

1、QPixmap依赖于硬件，QImage不依赖;

2、QPixmap主要是用于绘图，针对屏幕显示而最佳化设计，QImage主要是为图像IO、图片访问和像素修改设计的。

3、总结：当图片比较小的时候，直接使用QPixmap进行加载，画图时无所谓，当图片大的时候如果直接用QPixmap进行加载会占用比较大的内存，所以当图片大的时候用QImage进行加载，然后转成QPixmap绘制。QPixmap绘制效果最好。
