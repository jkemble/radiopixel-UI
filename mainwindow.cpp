#include <QGridLayout>
#include <QTabWidget>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QTextStream>
#include "mainwindow.h"
#ifdef SERIAL
#include <QSerialPortInfo>
#endif

QUuid uart( 0x6E400001, 0xB5A3, 0xF393, 0xe0, 0xA9, 0xe5, 0x0E, 0x24, 0xDC, 0xCA, 0x9E);
QUuid uartTx( 0x6E400002, 0xB5A3, 0xF393, 224, 0xA9, 0xe5, 0x0E, 0x24, 0xDC, 0xCA, 0x9E );
QUuid uartRx( 0x6E400003, 0xB5A3, 0xF393, 224, 0xA9, 0xe5, 0x0E, 0x24, 0xDC, 0xCA, 0x9E );


void Num::setValue(int value)
{
    setText( QString::number(value));
}

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    // setup macros
    const int FULL = 128;
    //Step( duration(secs), brightness(0-255), speed(%), level1, pattern, color1, color2, color3 )
    Step idle( 0, 20, 35, 17, 7, Qt::red, Qt::white, Qt::green );
    {
        Sequence m( "Warn", Qt::yellow );
        m.AddStep( Step(  4, FULL, 100, 255, 4, Qt::yellow, Qt::yellow, Qt::yellow ) );
        m.AddStep( Step( 60, FULL,  40,  34, 5, Qt::yellow, Qt::yellow, Qt::yellow ) );
        m.AddStep( Step( 60, FULL, 100,  75, 0, Qt::yellow, QColor( 255, 255, 64 ), Qt::yellow ) );
        m.AddStep( Step(  0, FULL/2,  75,  75, 7, Qt::yellow, QColor( 255, 255, 64 ), Qt::yellow ) );
        m_macros.push_back( m );
    }
    {
        Sequence m( "Exit", Qt::red );
        m.AddStep( Step(  4, FULL, 100, 255, 4, Qt::red, Qt::red, Qt::red ) );
        m.AddStep( Step( 60, FULL,  40,  34, 5, Qt::red, Qt::red, Qt::red ) );
        m.AddStep( Step( 60, FULL, 100,  75, 0, Qt::red, QColor( 255, 64, 64 ), Qt::red ) );
        m.AddStep( Step(  0, FULL/2,  75,  75, 7, Qt::red, QColor( 255, 64, 64 ), Qt::red ) );
        m_macros.push_back( m );
    }
    {
        Sequence m( "Idle", Qt::white );
        m.AddStep( idle );
        m_macros.push_back( m );
    }

    {
        Sequence m( "RWR\nSubtle" );
        m.AddStep( Step( 30, FULL, 35, 17, 7, Qt::red, Qt::white, Qt::red ) );
        m.AddStep( idle );
        m_macros.push_back( m );
    }
    {
        Sequence m( "Blue\nSmooth");
        m.AddStep( Step( 30, FULL,  75,  75, 7, Qt::blue, QColor( 128, 128, 255 ), Qt::blue ) );
        m.AddStep( idle );
        m_macros.push_back( m );
    }
    {
        Sequence m( "RWB\nParis USA");
        m.AddStep( Step( 10, FULL, 160, 160, 0, Qt::red, Qt::white, Qt::blue ) );
        m.AddStep( idle );
        m_macros.push_back( m );
    }

    {
        Sequence m( "RWG\nCandy");
        m.AddStep( Step( 30, FULL/2, 65, 255, 10, Qt::red, Qt::white, Qt::green ) );
        m.AddStep( idle );
        m_macros.push_back( m );
    }    
    {
        Sequence m( "RWR\nCandy");
        m.AddStep( Step( 30, FULL/2, 100, 255, 10, Qt::red, Qt::white, Qt::red ) );
        m.AddStep( idle );
        m_macros.push_back( m );
    }
    {
        Sequence m( "RWG\nTree");
        m.AddStep( Step( 10, FULL, 100, 255, 8, Qt::red, Qt::white, Qt::green ) );
        m.AddStep( idle );
        m_macros.push_back( m );
    }

    {
        Sequence m( "RWG\nMarch");
        m.AddStep( Step( 30, FULL, 127, 8, 5, Qt::red, Qt::white, Qt::green ) );
        m.AddStep( idle );
        m_macros.push_back( m );
    }
    {
        Sequence m( "RWG\nWipe");
        m.AddStep( Step( 30, FULL, 127, 8, 6, Qt::red, Qt::white, Qt::green ) );
        m.AddStep( idle );
        m_macros.push_back( m );
    }
    {
        Sequence m( "RWG\nFlicker");
        m.AddStep( Step( 10, FULL, 255, 9, 1, Qt::red, Qt::white, Qt::green ) );
        m.AddStep( idle );
        m_macros.push_back( m );
    }

    {
        Sequence m( "CGA");
        m.AddStep( Step( 30, FULL, 100, 128, 0, Qt::cyan, Qt::magenta, Qt::yellow ) );
        m.AddStep( idle );
        m_macros.push_back( m );
    }
    {
        Sequence m( "Rainbow");
        m.AddStep( Step( 10, FULL, 100, 255, 3, Qt::white, Qt::white, Qt::white ) );
        m.AddStep( idle );
        m_macros.push_back( m );
    }
    {
        Sequence m( "Strobe");
        m.AddStep( Step( 10, FULL, 128, 255, 9, Qt::white, Qt::white, Qt::white ) );
        m.AddStep( idle );
        m_macros.push_back( m );
    }

    m_macroTimer.setSingleShot( true );
    connect( &m_macroTimer, SIGNAL(timeout()), this, SLOT(StepMacro()));

    // tab
    QVBoxLayout *vb = new QVBoxLayout();
    setLayout( vb);
    QTabWidget *tabs = new QTabWidget();
    vb->addWidget( tabs);

    // presets
    QWidget *page = new QWidget();
    {
        QGridLayout *grid = new QGridLayout( );
        page->setLayout( grid );

        for ( int m = 0; m < m_macros.size(); ++m)
        {
            QPushButton *button = new QPushButton( m_macros.at( m ).name );
            button->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
            QColor color( m_macros.at( m ).color );
            if ( color.isValid())
            {
                /*
                QPalette pal( button->palette());
                pal.setBrush( QPalette::Button, color );
                button->setPalette( pal );
                //button->setAutoFillBackground( true );
                */
                button->setStyleSheet( QString("background-color: %1;").arg(color.name()));
            }
            grid->addWidget( button, m / 3, m % 3 );
            m_macroGroup.addButton( button, m );
        }

        connect( &m_macroGroup, SIGNAL(buttonClicked(int)), this, SLOT(StartMacro( int)));
    }
    tabs->addTab( page, "Presets" );

    // manual
    page = new QWidget();
    {
        QGridLayout *grid = new QGridLayout( );
        page->setLayout( grid );

        int r = 0;

        // ports
        m_ports = new QComboBox( );
        connect( m_ports, SIGNAL(activated(int)),
                 this, SLOT( ChangePort(int)));
        grid->addWidget( m_ports, r, 0, 1, 8 );

        QPushButton *refresh = new QPushButton( "R" );
        connect( refresh, SIGNAL(clicked(bool)),
                 this, SLOT( discover()));
        grid->addWidget( refresh, r, 8, 1, 1 );

        r++;

        // colors
        QGridLayout *hbox = new QGridLayout( );
        {
            m_color1 = new ColorPicker();
            m_color1->setColor( Qt::red );
            connect( m_color1, SIGNAL(changed(QColor)), this, SLOT(SendPattern()));
            hbox->addWidget( m_color1, 0, 0 );
            m_color2 = new ColorPicker();
            m_color2->setColor( Qt::white );
            connect( m_color2, SIGNAL(changed(QColor)), this, SLOT(SendPattern()));
            hbox->addWidget( m_color2, 0, 1 );
            m_color3 = new ColorPicker();
            m_color3->setColor( Qt::green );
            connect( m_color3, SIGNAL(changed(QColor)), this, SLOT(SendPattern()));
            hbox->addWidget( m_color3, 0, 2 );

            //hbox->addWidget( new QLabel( "color 1"), 1, 0, Qt::AlignCenter );
            //hbox->addWidget( new QLabel( "color 2"), 1, 1, Qt::AlignCenter );
            //hbox->addWidget( new QLabel( "color 3"), 1, 2, Qt::AlignCenter );
        }
        grid->addLayout( hbox, r, 0, 1, 9 );
        r++;

        // master
        m_brightness = new QSlider( );
        m_brightness->setRange( 0, 255 );
        m_brightness->setValue( 128 );
        m_brightness->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
        connect( m_brightness, SIGNAL(valueChanged(int)), this, SLOT(SendControl()));
        grid->addWidget( m_brightness, r, 0, 5, 1);

        m_speed = new QSlider( );
        m_speed->setRange( 0, 255 );
        m_speed->setValue( 100 );
        m_speed->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
        connect( m_speed, SIGNAL(valueChanged(int)), this, SLOT(SendControl()));
        grid->addWidget( m_speed, r, 1, 5, 1);

        // levels
        m_level1 = new QSlider( );
        m_level1->setRange( 0, 255 );
        m_level1->setValue( 128 );
        m_level1->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
        connect( m_level1, SIGNAL(valueChanged(int)), this, SLOT(SendPattern()));
        grid->addWidget( m_level1, r, 3, 5, 1);

        m_level2 = new QSlider( );
        m_level2->setRange( 0, 255 );
        m_level2->setValue( 128 );
        m_level2->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
        connect( m_level2, SIGNAL(valueChanged(int)), this, SLOT(SendPattern()));
//        grid->addWidget( m_level2, r, 4, 5, 1);
        m_level2->hide();

        m_level3 = new QSlider( );
        m_level3->setRange( 0, 255 );
        m_level3->setValue( 128 );
        m_level3->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
        connect( m_level3, SIGNAL(valueChanged(int)), this, SLOT(SendPattern()));
//        grid->addWidget( m_level3, r, 5, 5, 1);
        m_level3->hide();

        // patterns
        QStringList names;
        names << "MiniTw" << "MiniSp" << "Sparkle" << "RainBow" << "Flash"
              << "March" << "Wipe" << "Gradient" << "Fixed" << "Strobe" << "Candy";
        for ( int i = 0; i < names.size(); ++i )
        {
            const int rows = 6;
            QPushButton *button = new QPushButton( names.at( i ) );
            button->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
            button->setCheckable( true );
            m_pattern.addButton( button, i );
            grid->addWidget( button, r + (i%rows), 5 + (i/rows)*2, 1, 2 );
        }
        m_pattern.button( 0 )->setChecked( true );
        connect( &m_pattern, SIGNAL(buttonClicked(int)), this, SLOT(SendPattern()));
        r += 5;

        // values
        Num *num = new Num();
        connect( m_brightness, SIGNAL(valueChanged(int)),
                 num, SLOT(setValue(int)));
        grid->addWidget( num, r, 0, Qt::AlignCenter );
        num = new Num();
        connect( m_speed, SIGNAL(valueChanged(int)),
                 num, SLOT(setValue(int)));
        grid->addWidget( num, r, 1, Qt::AlignCenter );
        num = new Num();
        connect( m_level1, SIGNAL(valueChanged(int)),
                 num, SLOT(setValue(int)));
        grid->addWidget( num, r, 3, Qt::AlignCenter );
/*
        num = new Num();
        connect( m_level2, SIGNAL(valueChanged(int)),
                 num, SLOT(setValue(int)));
        grid->addWidget( num, r, 4, Qt::AlignCenter );
        num = new Num();
        connect( m_level3, SIGNAL(valueChanged(int)),
                 num, SLOT(setValue(int)));
        grid->addWidget( num, r, 5, Qt::AlignCenter );
*/
        r++;

        // labels
        grid->addWidget( new QLabel( "int"), r, 0, Qt::AlignCenter );
        grid->addWidget( new QLabel( "rate"), r, 1, Qt::AlignCenter );
        grid->addWidget( new QLabel( "level"), r, 3, Qt::AlignCenter );
        //grid->addWidget( new QLabel( "patterns"), r, 5, 1, 4, Qt::AlignCenter );
        r++;

        // feedback
        m_recv = new QTextEdit( );
        grid->addWidget( m_recv, r, 0, 1, 9 );
        r++;

        for ( int i = 0; i < 9; ++i )
        {
            grid->setColumnStretch( i, 1);
        }
    }
    tabs->addTab( page, "Manual" );

    // serial
#ifdef SERIAL
    connect( &m_port, SIGNAL(readyRead()), this, SLOT( ReadSerial()));
#endif

    // bluetooth
    connect( &m_btAgent, SIGNAL( deviceDiscovered( QBluetoothDeviceInfo ) ),
        this, SLOT( deviceDiscovered( QBluetoothDeviceInfo ) ) );
    connect( &m_btAgent, SIGNAL( finished() ),
        this, SLOT( deviceDone( ) ) );
    discover();

    connect( &m_xmitTimer, SIGNAL(timeout()), this, SLOT(SendPending()));
}

MainWindow::~MainWindow()
{

}

void MainWindow::discover( )
{
    m_ports->clear();

#ifdef SERIAL
    m_serialPorts = QSerialPortInfo::availablePorts( );
    for( int i = 0; i < m_serialPorts.size(); ++i )
    {
        const QSerialPortInfo& info( m_serialPorts.at( i ) );
        QString str( "S%1");
        m_ports->addItem( info.portName(), str.arg( i ));
    }
    int i = m_ports->findText( "usbserial-DJ", Qt::MatchContains );
    if ( i != -1 )
    {
        ChangePort( i );
    }
#endif

    m_btDevices.clear();
    Log("BT device discovery starting\n");
    m_btAgent.start();
}

void MainWindow::deviceDiscovered( QBluetoothDeviceInfo info )
{
    Log(QString("BT device %1 found\n").arg(info.name()));
    QString str( "B%1");
    m_ports->addItem( info.name( ), str.arg( m_btDevices.size() ) );
    m_btDevices.push_back( info );
}

void MainWindow::deviceDone( )
{
    Log("BT device discovery done\n");
    int i = m_ports->findText( "Adafruit Bluefruit LE", Qt::MatchStartsWith );
    if ( i != -1 )
    {
        ChangePort( i );
    }
}

void MainWindow::deviceConnected()
{
    Log("BT connect done\n");
    Log("BT service discovery starting\n");
    m_btController->discoverServices();
}

void MainWindow::serviceDiscovered(const QBluetoothUuid &gatt)
{
    if ( gatt == uart )
    {
        Log("BT service UART found\n");
    }
    else
    {
        Log( QString( "BT service %1 found\n").arg(gatt.toString()));
    }
}

void MainWindow::servicesDone()
{
    Log("BT service discovery done\n");
    m_btService = m_btController->createServiceObject( uart );
    if ( !m_btService)
    {
        Log("BT can't find UART service\n");
        return;
    }

    connect( m_btService, SIGNAL(stateChanged(QLowEnergyService::ServiceState)),
             this, SLOT(serviceStateChanged(QLowEnergyService::ServiceState)));
    Log("BT detail discovery starting\n");
    m_btService->discoverDetails();
}

void MainWindow::serviceStateChanged( QLowEnergyService::ServiceState s )
{
    Log(QString("BT service state %1\n").arg(s));
}

void MainWindow::ChangePort(int index)
{
    QString str( m_ports->itemData( index ).toString());
    QChar type( str.at( 0 ) );
    int i( str.mid( 1 ).toInt( ) );
    if ( type == 'B' )
    {
        QBluetoothDeviceInfo info( m_btDevices.at( i ) );
        delete m_btController;
        m_btController = new QLowEnergyController( info, this );
        connect( m_btController, SIGNAL(connected()),
                 this, SLOT(deviceConnected()));
        connect( m_btController, SIGNAL(serviceDiscovered(const QBluetoothUuid &)),
                 this, SLOT(serviceDiscovered(const QBluetoothUuid &)));
        connect( m_btController, SIGNAL(discoveryFinished()),
                 this, SLOT(servicesDone()));
        Log("BT connect started\n");
        m_btController->connectToDevice( );
        delete m_btService;
    }
    else if ( type == 'S' )
    {
#ifdef SERIAL
        QSerialPortInfo info( m_serialPorts.at( i ) );
        m_port.setPort( info );
        if ( !m_port.open( QIODevice::ReadWrite ) )
        {
            QMessageBox msg( QMessageBox::Critical, "error", m_port.errorString());
            msg.exec();
            return;
        }

        m_port.setBaudRate( 9600 );
#endif
    }

    SendPattern( );
}

void MainWindow::SendControl( )
{
    HatPacket packet;
    packet.command = HC_CONTROL;
    packet.brightness = m_brightness->value();
    packet.speed = m_speed->value();

    SendPacket( packet );
}

void MainWindow::SendPattern( )
{
    // stop any running macros
    m_macroTimer.stop();

    HatPacket packet;
    packet.command = HC_PATTERN;
    packet.pattern = m_pattern.checkedId( );
    packet.brightness = m_brightness->value();
    packet.speed = m_speed->value();
    packet.color[0] = m_color1->color().rgb();
    packet.color[1] = m_color2->color().rgb();
    packet.color[2] = m_color3->color().rgb();
    packet.level[0] = m_level1->value();
    packet.level[1] = m_level2->value();
    packet.level[2] = m_level3->value();

    SendPacket( packet );
}

void MainWindow::ReadSerial()
{
    QString buf;

#ifdef SERIAL
    buf = m_port.readAll();
#endif

    Log( buf);
}

void MainWindow::StartMacro( int macro )
{
    ExecMacroStep( macro, 0 );
}

void MainWindow::StepMacro( )
{
    ExecMacroStep( m_macro, m_macroStep + 1 );
}

void MainWindow::ExecMacroStep( int _macro, int _step )
{
    // validate steps
    if ( _macro >= m_macros.size( ) )
    {
        return;
    }
    m_macro = _macro;
    Sequence& macro( m_macros[ m_macro ] );
    if ( _step >= macro.steps.size( ) )
    {
        return;
    }
    m_macroStep = _step;
    Step& step( macro.steps[ m_macroStep ] );

    // start timer
    if ( step.duration )
    {
        m_macroTimer.start( step.duration * 1000 );
    }
    else
    {
        m_macroTimer.stop();
    }

    // transmit
    //step.pattern.brightness = m_brightness->value();
    //step.pattern.speed = m_speed->value();
    SendPacket( step.pattern );
}

void MainWindow::SendPending( )
{
    SendPacket( m_pending );
}

void MainWindow::SendPacket( const HatPacket& packet )
{
    // if we're being throttled then get out now
    if ( m_xmitTimer.isActive())
    {
        m_pending = packet;
        return;
    }

    // if it's too soon, start throttling
    time_t now = time( NULL );
    if ( now == m_xmitLast )
    {
        m_pending = packet;
        m_xmitTimer.setSingleShot( true );
        m_xmitTimer.start( 1000 );
        return;
    }

    m_xmitLast = now;

    // build buffer
    QByteArray data;
//    data.append( 'Z');
    data.append( ( char *)&packet, sizeof packet );

    // transmit
    if ( m_btService )
    {
        QLowEnergyService::ServiceState state = m_btService->state();
        if ( state == QLowEnergyService::ServiceDiscovered)
        {
            QLowEnergyCharacteristic ch( m_btService->characteristic( uartTx ) );
            const int maxlen = 20;
            for ( int i = 0; i < data.size(); i += maxlen)
            {
                QByteArray part( data.data() + i, std::min( maxlen, data.size() - i ) );
                m_btService->writeCharacteristic( ch, part, QLowEnergyService::WriteWithoutResponse );
            }
        }
    }
    else
    {
#ifdef SERIAL
        m_port.write( data );
#endif
    }
}

void MainWindow::Log(const QString& str)
{
    QTextCursor cursor( m_recv->document( ) );
    cursor.movePosition( QTextCursor::End );
    cursor.insertText( str);
}
