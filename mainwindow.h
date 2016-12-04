#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#define SERIAL
#ifdef SERIAL
#include <vector>
#include <QSerialPort>
#endif
#include <QBluetoothDeviceDiscoveryAgent>
#include <QLowEnergyController>
#include <QPointer>
#include <QTimer>
#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QTextEdit>
#include <QSignalMapper>
#include <QSlider>
#include <QButtonGroup>
#include "colorpicker.h"
#include "radiopixel-protocol/radiopixel_protocol.h"


struct Step
{
    Step( int _duration, int _brightness, int _speed, uint8_t level1, int _pattern,
          QColor color1, QColor color2, QColor color3,
          uint8_t level2 = 255, uint8_t level3 = 255 )
        : pattern( _brightness, _speed, _pattern, color1.rgb(), color2.rgb(), color3.rgb(), level1, level2, level3 ),
          duration( _duration )
    {
    }

    HatPacket pattern;
    int duration;
};

typedef std::vector< Step > Steps;


class Sequence
{
public:
    Sequence( QString _name, QColor _color = QColor( ) )
        : name( _name ), color( _color )
    {
    }

    void AddStep( Step step )
    {
        steps.push_back( step );
    }

    QString name;
    QColor color;
    Steps steps;
};

typedef std::vector< Sequence > Macros;

class Num : public QLabel
{
    Q_OBJECT

public slots:
    void setValue( int );
};

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void discover();
    void deviceDiscovered( QBluetoothDeviceInfo info );
    void deviceDone();
    void deviceConnected();
    void serviceDiscovered( const QBluetoothUuid &gatt );
    void servicesDone( );
    void serviceStateChanged( QLowEnergyService::ServiceState s );

    void ChangePort( int index );

    void SendControl( );
    void SendPattern( );

    void ReadSerial( );

    void StartMacro( ) { StartMacro( 0); }
    void StartMacro( int macro );
    void StepMacro( );

protected slots:
    void SendPending( );

protected:
    void ExecMacroStep( int macro, int step );
    void SendPacket( const HatPacket& packet );
    void Log( const QString& str );

    // serial ports
#ifdef SERIAL
    QList< QSerialPortInfo > m_serialPorts;
    QSerialPort m_port;
#endif

    // bluetooth devices
    QBluetoothDeviceDiscoveryAgent m_btAgent;
    QList< QBluetoothDeviceInfo > m_btDevices;
    QPointer< QLowEnergyController > m_btController;
    QPointer< QLowEnergyService > m_btService;

    // GUI
    QComboBox *m_ports;
    ColorPicker *m_color1, *m_color2, *m_color3;
    QSlider *m_brightness, *m_speed, *m_level1, *m_level2, *m_level3;
    QButtonGroup m_pattern, m_macroGroup;
    QTextEdit *m_recv;

    // Transmit handling
    HatPacket m_pending;
    QTimer m_xmitTimer;
    time_t m_xmitLast;

    // Macros
    Macros m_macros;
    QTimer m_macroTimer;
    int m_macro;
    int m_macroStep;
};

#endif // MAINWINDOW_H
