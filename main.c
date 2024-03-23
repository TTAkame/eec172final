
// Standard includes
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "interrupt.h"
#include "hw_apps_rcm.h"
#include "prcm.h"
#include "rom.h"
#include "rom_map.h"
#include "prcm.h"
#include "gpio.h"
#include "timer.h"
#include "uart.h"
#include "utils.h"
#include "spi.h"
#include "simplelink.h"
#include "uart_if.h"
#include "i2c_if.h"

// Common interface includes
#include "uart_if.h"
#include "timer_if.h"
#include "gpio_if.h"
#include "common.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1351.h"
#include "test.h"

#include "pin_mux_config.h"
#include <stdbool.h> // For boolean type variables

// Adjusted constants for the 128x128 display
#define PADDLE_WIDTH 10
#define PADDLE_HEIGHT 4
#define BALL_SIZE 5  // Increase ball size for better visibility
#define BALL_VELOCITY 3  // Adjust ball velocity for engaging gameplay
#define BRICK_ROWS 6  // Increase rows to utilize vertical space
#define BRICK_COLUMNS 8  // Keep columns to fit screen width
#define BRICK_WIDTH (WIDTH / BRICK_COLUMNS)
#define BRICK_HEIGHT 10
#define NO_OF_BRICKS (BRICK_ROWS * BRICK_COLUMNS)

// Game variables
int paddleX, paddleY;
int ballX, ballY;
int ballVelocityX, ballVelocityY;
bool bricks[BRICK_ROWS][BRICK_COLUMNS]; // true if the brick is visible
int prevBallX, prevBallY;
int prevPaddleX;
int gameoverbreak;
int dx = 0;
int pause;
int start;

bool brickNeedsRedraw[BRICK_ROWS][BRICK_COLUMNS] = {false};


#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))


#define SPI_IF_BIT_RATE  100000
#define CONSOLE1         UARTA1_BASE
#define CONSOLE1_PERIPH  PRCM_UARTA1
#define ZERO "01100000011000001001000001101111"
#define ONE "01100000011000000000000011111111"
#define TWO "01100000011000001000000001111111"
#define THREE "01100000011000000100000010111111"
#define FOUR "01100000011000001100000000111111"
#define FIVE "01100000011000000010000011011111"
#define SIX "01100000011000001010000001011111"
#define SEVEN "01100000011000000110000010011111"
#define EIGHT "01100000011000001110000000011111"
#define NINE "01100000011000000001000011101111"
#define MUTE "01100000011000000111000010001111"
#define LAST "01100000011000000000100011110111"
#define MAX_URI_SIZE 128
#define URI_SIZE MAX_URI_SIZE + 1


#define APPLICATION_NAME        "SSL"
#define APPLICATION_VERSION     "1.1.1.EEC.Spring2024"
#define SERVER_NAME             "a3cgotyp6wrk6k-ats.iot.us-east-2.amazonaws.com"
#define GOOGLE_DST_PORT         8443

#define SL_SSL_CA_CERT "/cert/rootCA.der" //starfield class2 rootca (from firefox) // <-- this one works
#define SL_SSL_PRIVATE "/cert/private.der"
#define SL_SSL_CLIENT  "/cert/client.der"


//NEED TO UPDATE THIS FOR IT TO WORK!
#define DATE                29    /* Current Date */
#define MONTH               2     /* Month 1-12 */
#define YEAR                2024  /* Current year */
#define HOUR                13    /* Time - hours */
#define MINUTE              46    /* Time - minutes */
#define SECOND              5     /* Time - seconds */

#define POSTHEADER "POST /things/Darcy_CC3200_Board/shadow HTTP/1.1\r\n"
#define HOSTHEADER "Host: a3cgotyp6wrk6k-ats.iot.us-east-2.amazonaws.com\r\n"
#define CHEADER "Connection: Keep-Alive\r\n"
#define CTHEADER "Content-Type: application/json; charset=utf-8\r\n"
#define CLHEADER1 "Content-Length: "
#define CLHEADER2 "\r\n\r\n"


#define PART1 "{\"state\": {\r\n\"desired\" : {\r\n\"var\" : \""
#define PART2 "\"\r\n}}}\r\n\r\n"


// Application specific status/error codes
typedef enum{
    // Choosing -0x7D0 to avoid overlap w/ host-driver's error codes
    LAN_CONNECTION_FAILED = -0x7D0,
    INTERNET_CONNECTION_FAILED = LAN_CONNECTION_FAILED - 1,
    DEVICE_NOT_IN_STATION_MODE = INTERNET_CONNECTION_FAILED - 1,

    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;

typedef struct
{
   /* time */
   unsigned long tm_sec;
   unsigned long tm_min;
   unsigned long tm_hour;
   /* date */
   unsigned long tm_day;
   unsigned long tm_mon;
   unsigned long tm_year;
   unsigned long tm_week_day; //not required
   unsigned long tm_year_day; //not required
   unsigned long reserved[3];
}SlDateTime;


//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
extern void (* const g_pfnVectors[])(void);
// Globals used to save and restore interrupt settings.

unsigned long interval = 0;
int resetFlag = 0;
char input[33] = "";
char prevButton[33];
char message[50] = "";
int messageIndex = 0;
int bitlen = 0;
int buttonPressed = 0;
int multiPress;
int firstPress = 1;
int prevPress;
int colorChange = 0;
int delete = 0;
int spaceUsed = 0;

int i;
int newchar;
char* prevbutton;
int x = 0;
int y = 0;
int left = 0;
int right = 0;
int count = 0;


// 1 = Red, 2 = Orange, 3 = Yellow, 4 = Green, 5 = Blue
unsigned int colors[] = {RED, MAGENTA, YELLOW, GREEN, BLUE};
unsigned int appliedColors[50] = {0};
int colorIndex = 0;
int buttonIndex = 0;
char button2[] = "abc";
char button3[] = "def";
char button4[] = "ghi";
char button5[] = "jkl";
char button6[] = "mno";
char button7[] = "pqrs";
char button8[] = "tuv";
char button9[] = "wxyz";

char received[50];
int sendFlag, receiveFlag;
int receive;

volatile unsigned long  g_ulStatus = 0;//SimpleLink Status
unsigned long  g_ulPingPacketsRecv = 0; //Number of Ping Packets received
unsigned long  g_ulGatewayIP = 0; //Network Gateway IP address
unsigned char  g_ucConnectionSSID[SSID_LEN_MAX+1]; //Connection SSID
unsigned char  g_ucConnectionBSSID[BSSID_LEN_MAX]; //Connection BSSID
signed char    *g_Host = SERVER_NAME;
SlDateTime g_time;
#if defined(ccs) || defined(gcc)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif


//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

typedef struct PinSetting {
    unsigned long port;
    unsigned int pin;
} PinSetting;

static PinSetting IR_IN = { .port = GPIOA2_BASE, .pin = 0x2};

//*****************************************************************************
//                      LOCAL FUNCTION PROTOTYPES
//*****************************************************************************
static void BoardInit(void);
static long WlanConnect();
static int set_time();
static long InitializeAppVariables();
static int tls_connect();
static int connectToAccessPoint();
static int http_post(int);

void initGame() {
    fillScreen(BLACK);
    paddleX = (WIDTH - PADDLE_WIDTH) / 2;
    paddleY = HEIGHT - 15;  // Position paddle a bit higher for more play area
    ballX = WIDTH / 2;
    ballY = paddleY - 10;  // Start the ball just above the paddle
    ballVelocityX = BALL_VELOCITY;
    ballVelocityY = -BALL_VELOCITY;
    int i,j;
    // Initialize bricks to be visible
    for (i = 0; i < BRICK_ROWS; i++) {
        for (j = 0; j < BRICK_COLUMNS; j++) {
            bricks[i][j] = true;
        }
    }
    prevBallX = ballX;
    prevBallY = ballY;
    prevPaddleX = paddleX;
    // Initialize brickNeedsRedraw to false for all bricks
    memset(brickNeedsRedraw, false, sizeof(brickNeedsRedraw));
}

// Function to draw the paddle
void drawBall() {
    clearBall(); // Clear ball's previous position
    fillCircle(ballX, ballY, BALL_SIZE / 2, WHITE);
    prevBallX = ballX;
    prevBallY = ballY;
}

void drawPaddle() {
    if (paddleX != prevPaddleX) {
        clearPaddle(); // Only clear if the paddle has moved
    }
    fillRect(paddleX, paddleY, PADDLE_WIDTH, PADDLE_HEIGHT, WHITE);
    prevPaddleX = paddleX;
}


// Function to draw bricks
void drawBricks() {
    int i,j;
    for (i = 0; i < BRICK_ROWS; i++) {
        for (j = 0; j < BRICK_COLUMNS; j++) {
            if (bricks[i][j]) {
                int x = j * BRICK_WIDTH;
                int y = i * BRICK_HEIGHT;
                drawRect(x, y, BRICK_WIDTH - 1, BRICK_HEIGHT - 1, WHITE);
            }
        }
    }
}
int getDelta(signed char Magnitude){
    //this function calculates the change in coordinates for the ball based on the tilt value (Magnitude)
    float max = 64; //value to divide the tilt by. can be any arbitrary value.
    //get delta by dividing tilt value by 64
    float delta = ((float)Magnitude) / max;
    //multiply delta by 10 so that the ball will move faster
    delta =delta*10;
    return (int) delta;
}

// Function to update the game state
void updateGame() {
    int  i,j;
    // Move the ball
    ballX += ballVelocityX;
    ballY += ballVelocityY;

    // Collision detection with walls
    if (ballX <= 0 || ballX >= WIDTH - BALL_SIZE) {
        ballVelocityX = -ballVelocityX; // Reverse X direction
    }
    if (ballY <= 0) {
        ballVelocityY = -ballVelocityY; // Reverse Y direction
    }

    // Collision detection with the paddle
    if (ballY >= paddleY - BALL_SIZE && ballX >= paddleX && ballX <= paddleX + PADDLE_WIDTH) {
        ballVelocityY = -ballVelocityY; // Reverse Y direction
    }

    // Collision detection with bricks
    bool brickHit = false;
    for (i = 0; i < BRICK_ROWS && !brickHit; i++) {
        for (j = 0; j < BRICK_COLUMNS && !brickHit; j++) {
            if (bricks[i][j]) {
                // Calculate the edges of the ball
                int ballLeft = ballX;
                int ballRight = ballX + BALL_SIZE;
                int ballTop = ballY;
                int ballBottom = ballY + BALL_SIZE;

                // Brick dimensions
                int brickLeft = j * BRICK_WIDTH;
                int brickRight = brickLeft + BRICK_WIDTH;
                int brickTop = i * BRICK_HEIGHT;
                int brickBottom = brickTop + BRICK_HEIGHT;

                // Check for collision
                if (ballBottom >= brickTop && ballTop <= brickBottom && ballRight >= brickLeft && ballLeft <= brickRight) {
                    // Collision occurred, determine which side of the brick was hit
                    int overlapLeft = ballRight - brickLeft;
                    int overlapRight = brickRight - ballLeft;
                    int overlapTop = ballBottom - brickTop;
                    int overlapBottom = brickBottom - ballTop;

                    int minOverlap = min(overlapLeft, min(overlapRight, min(overlapTop, overlapBottom)));

                    // Adjust ball position based on the side of the brick hit
                    if (minOverlap == overlapLeft)
                        ballX -= overlapLeft;
                    else if (minOverlap == overlapRight)
                        ballX += overlapRight;
                    else if (minOverlap == overlapTop)
                        ballY -= overlapTop;
                    else if (minOverlap == overlapBottom)
                        ballY += overlapBottom;

                    // Reflect ball
                    if (minOverlap == overlapLeft || minOverlap == overlapRight)
                        ballVelocityX = -ballVelocityX;
                    else
                        ballVelocityY = -ballVelocityY;

                    bricks[i][j] = false; // The brick is destroyed
                    brickNeedsRedraw[i][j] = true; // Mark for redraw
                    brickHit = true;
                }
            }
        }
    }


    // Game over condition (ball falls below paddle)
    if (ballY > HEIGHT) {
        // Optionally, implement game over logic here
        gameoverbreak = 1;
        initGame(); // For now, simply reset the game

    }

    // After processing collisions and movements...
    drawBall(); // Redraw the ball at its new position
    drawPaddle(); // Update the paddle position if it has moved
    updateBricks(); // Redraw any bricks that have been hit
}

void handlePaddleMovement(int x) {

    const int paddleMoveAmount = 5; // Adjust for desired paddle speed

    // Move paddle left
    if (x <0 && paddleX > 0) {
        paddleX -= paddleMoveAmount;
        if (paddleX < 0) {
            paddleX = 0; // Prevent the paddle from moving out of bounds
        }
    }

    // Move paddle right
    else if (x > 0 && paddleX < WIDTH - PADDLE_WIDTH) {
        paddleX += paddleMoveAmount;
        if (paddleX > WIDTH - PADDLE_WIDTH) {
            paddleX = WIDTH - PADDLE_WIDTH; // Prevent the paddle from moving out of bounds
        }
    }
}

void clearBall() {
    fillCircle(prevBallX, prevBallY, BALL_SIZE / 2, BLACK);
}

void clearPaddle() {
    fillRect(prevPaddleX, paddleY, PADDLE_WIDTH, PADDLE_HEIGHT, BLACK);
}
void updateBricks() {
    int i,j;
    for (i = 0; i < BRICK_ROWS; i++) {
        for (j = 0; j < BRICK_COLUMNS; j++) {
            if (brickNeedsRedraw[i][j]) {
                int x = j * BRICK_WIDTH;
                int y = i * BRICK_HEIGHT;
                drawRect(x, y, BRICK_WIDTH - 1, BRICK_HEIGHT - 1, bricks[i][j] ? WHITE : BLACK);
                brickNeedsRedraw[i][j] = false; // Reset redraw flag
            }
        }
    }
}


//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- Start
//*****************************************************************************


//*****************************************************************************
//
//! \brief The Function Handles WLAN Events
//!
//! \param[in]  pWlanEvent - Pointer to WLAN Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent) {
    if(!pWlanEvent) {
        return;
    }

    switch(pWlanEvent->Event) {
        case SL_WLAN_CONNECT_EVENT: {
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);

            //
            // Information about the connected AP (like name, MAC etc) will be
            // available in 'slWlanConnectAsyncResponse_t'.
            // Applications can use it if required
            //
            //  slWlanConnectAsyncResponse_t *pEventData = NULL;
            // pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
            //

            // Copy new connection SSID and BSSID to global parameters
            memcpy(g_ucConnectionSSID,pWlanEvent->EventData.
                   STAandP2PModeWlanConnected.ssid_name,
                   pWlanEvent->EventData.STAandP2PModeWlanConnected.ssid_len);
            memcpy(g_ucConnectionBSSID,
                   pWlanEvent->EventData.STAandP2PModeWlanConnected.bssid,
                   SL_BSSID_LENGTH);

            Report("[WLAN EVENT] STA Connected to the AP: %s , "
                       "BSSID: %x:%x:%x:%x:%x:%x\n\r",
                       g_ucConnectionSSID,g_ucConnectionBSSID[0],
                       g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                       g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                       g_ucConnectionBSSID[5]);
        }
        break;

        case SL_WLAN_DISCONNECT_EVENT: {
            slWlanConnectAsyncResponse_t*  pEventData = NULL;

            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

            pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

            // If the user has initiated 'Disconnect' request,
            //'reason_code' is SL_USER_INITIATED_DISCONNECTION
            if(SL_USER_INITIATED_DISCONNECTION == pEventData->reason_code) {
                Report("[WLAN EVENT]Device disconnected from the AP: %s,"
                    "BSSID: %x:%x:%x:%x:%x:%x on application's request \n\r",
                           g_ucConnectionSSID,g_ucConnectionBSSID[0],
                           g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                           g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                           g_ucConnectionBSSID[5]);
            }
            else {
                Report("[WLAN ERROR]Device disconnected from the AP AP: %s, "
                           "BSSID: %x:%x:%x:%x:%x:%x on an ERROR..!! \n\r",
                           g_ucConnectionSSID,g_ucConnectionBSSID[0],
                           g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                           g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                           g_ucConnectionBSSID[5]);
            }
            memset(g_ucConnectionSSID,0,sizeof(g_ucConnectionSSID));
            memset(g_ucConnectionBSSID,0,sizeof(g_ucConnectionBSSID));
        }
        break;

        default: {
            Report("[WLAN EVENT] Unexpected event [0x%x]\n\r",
                       pWlanEvent->Event);
        }
        break;
    }
}

//*****************************************************************************
//
//! \brief This function handles network events such as IP acquisition, IP
//!           leased, IP released etc.
//!
//! \param[in]  pNetAppEvent - Pointer to NetApp Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent) {
    if(!pNetAppEvent) {
        return;
    }

    switch(pNetAppEvent->Event) {
        case SL_NETAPP_IPV4_IPACQUIRED_EVENT: {
            SlIpV4AcquiredAsync_t *pEventData = NULL;

            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

            //Ip Acquired Event Data
            pEventData = &pNetAppEvent->EventData.ipAcquiredV4;

            //Gateway IP address
            g_ulGatewayIP = pEventData->gateway;

            Report("[NETAPP EVENT] IP Acquired: IP=%d.%d.%d.%d , "
                       "Gateway=%d.%d.%d.%d\n\r",
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,3),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,2),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,1),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,0),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,3),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,2),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,1),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,0));
        }
        break;

        default: {
            Report("[NETAPP EVENT] Unexpected event [0x%x] \n\r",
                       pNetAppEvent->Event);
        }
        break;
    }
}

//*****************************************************************************
//
//! \brief This function handles HTTP server events
//!
//! \param[in]  pServerEvent - Contains the relevant event information
//! \param[in]    pServerResponse - Should be filled by the user with the
//!                                      relevant response information
//!
//! \return None
//!
//****************************************************************************
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent, SlHttpServerResponse_t *pHttpResponse) {
    // Unused in this application
}

//*****************************************************************************
//
//! \brief This function handles General Events
//!
//! \param[in]     pDevEvent - Pointer to General Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent) {
    if(!pDevEvent) {
        return;
    }

    //
    // Most of the general errors are not FATAL are are to be handled
    // appropriately by the application
    //
    Report("[GENERAL EVENT] - ID=[%d] Sender=[%d]\n\n",
               pDevEvent->EventData.deviceEvent.status,
               pDevEvent->EventData.deviceEvent.sender);
}


//*****************************************************************************
//
//! This function handles socket events indication
//!
//! \param[in]      pSock - Pointer to Socket Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock) {
    if(!pSock) {
        return;
    }

    switch( pSock->Event ) {
        case SL_SOCKET_TX_FAILED_EVENT:
            switch( pSock->socketAsyncEvent.SockTxFailData.status) {
                case SL_ECLOSE: 
                    Report("[SOCK ERROR] - close socket (%d) operation "
                                "failed to transmit all queued packets\n\n", 
                                    pSock->socketAsyncEvent.SockTxFailData.sd);
                    break;
                default: 
                    Report("[SOCK ERROR] - TX FAILED  :  socket %d , reason "
                                "(%d) \n\n",
                                pSock->socketAsyncEvent.SockTxFailData.sd, pSock->socketAsyncEvent.SockTxFailData.status);
                  break;
            }
            break;

        default:
            Report("[SOCK EVENT] - Unexpected Event [%x0x]\n\n",pSock->Event);
          break;
    }
}


//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- End breadcrumb: s18_df
//*****************************************************************************


//*****************************************************************************
//
//! \brief This function initializes the application variables
//!
//! \param    0 on success else error code
//!
//! \return None
//!
//*****************************************************************************
static long InitializeAppVariables() {
    g_ulStatus = 0;
    g_ulGatewayIP = 0;
    g_Host = SERVER_NAME;
    memset(g_ucConnectionSSID,0,sizeof(g_ucConnectionSSID));
    memset(g_ucConnectionBSSID,0,sizeof(g_ucConnectionBSSID));
    return SUCCESS;
}
//*****************************************************************************
//! \brief This function puts the device in its default state. It:
//!           - Set the mode to STATION
//!           - Configures connection policy to Auto and AutoSmartConfig
//!           - Deletes all the stored profiles
//!           - Enables DHCP
//!           - Disables Scan policy
//!           - Sets Tx power to maximum
//!           - Sets power policy to normal
//!           - Unregister mDNS services
//!           - Remove all filters
//!
//! \param   none
//! \return  On success, zero is returned. On error, negative is returned
//*****************************************************************************
static long ConfigureSimpleLinkToDefaultState() {
    SlVersionFull   ver = {0};
    _WlanRxFilterOperationCommandBuff_t  RxFilterIdMask = {0};

    unsigned char ucVal = 1;
    unsigned char ucConfigOpt = 0;
    unsigned char ucConfigLen = 0;
    unsigned char ucPower = 0;

    long lRetVal = -1;
    long lMode = -1;

    lMode = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(lMode);

    // If the device is not in station-mode, try configuring it in station-mode 
    if (ROLE_STA != lMode) {
        if (ROLE_AP == lMode) {
            // If the device is in AP mode, we need to wait for this event 
            // before doing anything 
            while(!IS_IP_ACQUIRED(g_ulStatus)) {
#ifndef SL_PLATFORM_MULTI_THREADED
              _SlNonOsMainLoopTask(); 
#endif
            }
        }

        // Switch to STA role and restart 
        lRetVal = sl_WlanSetMode(ROLE_STA);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Stop(0xFF);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(lRetVal);

        // Check if the device is in station again 
        if (ROLE_STA != lRetVal) {
            // We don't want to proceed if the device is not coming up in STA-mode 
            return DEVICE_NOT_IN_STATION_MODE;
        }
    }
    
    // Get the device's version-information
    ucConfigOpt = SL_DEVICE_GENERAL_VERSION;
    ucConfigLen = sizeof(ver);
    lRetVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &ucConfigOpt, 
                                &ucConfigLen, (unsigned char *)(&ver));
    ASSERT_ON_ERROR(lRetVal);
    
    Report("Host Driver Version: %s\n\r",SL_DRIVER_VERSION);
    Report("Build Version %d.%d.%d.%d.31.%d.%d.%d.%d.%d.%d.%d.%d\n\r",
    ver.NwpVersion[0],ver.NwpVersion[1],ver.NwpVersion[2],ver.NwpVersion[3],
    ver.ChipFwAndPhyVersion.FwVersion[0],ver.ChipFwAndPhyVersion.FwVersion[1],
    ver.ChipFwAndPhyVersion.FwVersion[2],ver.ChipFwAndPhyVersion.FwVersion[3],
    ver.ChipFwAndPhyVersion.PhyVersion[0],ver.ChipFwAndPhyVersion.PhyVersion[1],
    ver.ChipFwAndPhyVersion.PhyVersion[2],ver.ChipFwAndPhyVersion.PhyVersion[3]);

    // Set connection policy to Auto + SmartConfig 
    //      (Device's default connection policy)
    lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION, 
                                SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Remove all profiles
    lRetVal = sl_WlanProfileDel(0xFF);
    ASSERT_ON_ERROR(lRetVal);

    

    //
    // Device in station-mode. Disconnect previous connection if any
    // The function returns 0 if 'Disconnected done', negative number if already
    // disconnected Wait for 'disconnection' event if 0 is returned, Ignore 
    // other return-codes
    //
    lRetVal = sl_WlanDisconnect();
    if(0 == lRetVal) {
        // Wait
        while(IS_CONNECTED(g_ulStatus)) {
#ifndef SL_PLATFORM_MULTI_THREADED
              _SlNonOsMainLoopTask(); 
#endif
        }
    }

    // Enable DHCP client
    lRetVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&ucVal);
    ASSERT_ON_ERROR(lRetVal);

    // Disable scan
    ucConfigOpt = SL_SCAN_POLICY(0);
    lRetVal = sl_WlanPolicySet(SL_POLICY_SCAN , ucConfigOpt, NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Set Tx power level for station mode
    // Number between 0-15, as dB offset from max power - 0 will set max power
    ucPower = 0;
    lRetVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, 
            WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *)&ucPower);
    ASSERT_ON_ERROR(lRetVal);

    // Set PM policy to normal
    lRetVal = sl_WlanPolicySet(SL_POLICY_PM , SL_NORMAL_POLICY, NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Unregister mDNS services
    lRetVal = sl_NetAppMDNSUnRegisterService(0, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Remove  all 64 filters (8*8)
    memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
    lRetVal = sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *)&RxFilterIdMask,
                       sizeof(_WlanRxFilterOperationCommandBuff_t));
    ASSERT_ON_ERROR(lRetVal);

    lRetVal = sl_Stop(SL_STOP_TIMEOUT);
    ASSERT_ON_ERROR(lRetVal);

    InitializeAppVariables();
    
    return lRetVal; // Success
}


//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void BoardInit(void) {
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
  //
  // Set vector table base
  //
#if defined(ccs)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}


//****************************************************************************
//
//! \brief Connecting to a WLAN Accesspoint
//!
//!  This function connects to the required AP (SSID_NAME) with Security
//!  parameters specified in te form of macros at the top of this file
//!
//! \param  None
//!
//! \return  0 on success else error code
//!
//! \warning    If the WLAN connection fails or we don't aquire an IP
//!            address, It will be stuck in this function forever.
//
//****************************************************************************
static long WlanConnect() {
    SlSecParams_t secParams = {0};
    long lRetVal = 0;

    secParams.Key = SECURITY_KEY;
    secParams.KeyLen = strlen(SECURITY_KEY);
    secParams.Type = SECURITY_TYPE;

    Report("Attempting connection to access point: ");
    Report(SSID_NAME);
    Report("... ...");
    lRetVal = sl_WlanConnect(SSID_NAME, strlen(SSID_NAME), 0, &secParams, 0);
    ASSERT_ON_ERROR(lRetVal);

    Report(" Connected!!!\n\r");


    // Wait for WLAN Event
    while((!IS_CONNECTED(g_ulStatus)) || (!IS_IP_ACQUIRED(g_ulStatus))) {
        // Toggle LEDs to Indicate Connection Progress
        _SlNonOsMainLoopTask();
        GPIO_IF_LedOff(MCU_IP_ALLOC_IND);
        MAP_UtilsDelay(800000);
        _SlNonOsMainLoopTask();
        GPIO_IF_LedOn(MCU_IP_ALLOC_IND);
        MAP_UtilsDelay(800000);
    }

    return SUCCESS;

}




long printErrConvenience(char * msg, long retVal) {
    Report(msg);
    GPIO_IF_LedOn(MCU_RED_LED_GPIO);
    return retVal;
}


//*****************************************************************************
//
//! This function updates the date and time of CC3200.
//!
//! \param None
//!
//! \return
//!     0 for success, negative otherwise
//!
//*****************************************************************************

static int set_time() {
    long retVal;

    g_time.tm_day = DATE;
    g_time.tm_mon = MONTH;
    g_time.tm_year = YEAR;
    g_time.tm_sec = HOUR;
    g_time.tm_hour = MINUTE;
    g_time.tm_min = SECOND;

    retVal = sl_DevSet(SL_DEVICE_GENERAL_CONFIGURATION,
                          SL_DEVICE_GENERAL_CONFIGURATION_DATE_TIME,
                          sizeof(SlDateTime),(unsigned char *)(&g_time));

    ASSERT_ON_ERROR(retVal);
    return SUCCESS;
}

//*****************************************************************************
//
//! This function demonstrates how certificate can be used with SSL.
//! The procedure includes the following steps:
//! 1) connect to an open AP
//! 2) get the server name via a DNS request
//! 3) define all socket options and point to the CA certificate
//! 4) connect to the server via TCP
//!
//! \param None
//!
//! \return  0 on success else error code
//! \return  LED1 is turned solid in case of success
//!    LED2 is turned solid in case of failure
//!
//*****************************************************************************
static int tls_connect() {
    SlSockAddrIn_t    Addr;
    int    iAddrSize;
    unsigned char    ucMethod = SL_SO_SEC_METHOD_TLSV1_2;
    unsigned int uiIP,uiCipher = SL_SEC_MASK_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA;
    
 //   unsigned int uiIP;
//    unsigned int uiCipher = SL_SEC_MASK_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA;
 //   unsigned int uiCipher = SL_SEC_MASK_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256;
    
    
    
// SL_SEC_MASK_SSL_RSA_WITH_RC4_128_SHA
// SL_SEC_MASK_SSL_RSA_WITH_RC4_128_MD5
// SL_SEC_MASK_TLS_RSA_WITH_AES_256_CBC_SHA
// SL_SEC_MASK_TLS_DHE_RSA_WITH_AES_256_CBC_SHA
// SL_SEC_MASK_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA
// SL_SEC_MASK_TLS_ECDHE_RSA_WITH_RC4_128_SHA
// SL_SEC_MASK_TLS_RSA_WITH_AES_128_CBC_SHA256
// SL_SEC_MASK_TLS_RSA_WITH_AES_256_CBC_SHA256
// SL_SEC_MASK_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256
// SL_SEC_MASK_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256 // does not work (-340, handshake fails)
    long lRetVal = -1;
    int iSockID;

    lRetVal = sl_NetAppDnsGetHostByName(g_Host, strlen((const char *)g_Host),
                                    (unsigned long*)&uiIP, SL_AF_INET);

    if(lRetVal < 0) {
        return printErrConvenience("Device couldn't retrieve the host name \n\r", lRetVal);
    }

    Addr.sin_family = SL_AF_INET;
    Addr.sin_port = sl_Htons(GOOGLE_DST_PORT);
    Addr.sin_addr.s_addr = sl_Htonl(uiIP);
    iAddrSize = sizeof(SlSockAddrIn_t);
    //
    // opens a secure socket 
    //
    iSockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, SL_SEC_SOCKET);
    if( iSockID < 0 ) {
        return printErrConvenience("Device unable to create secure socket \n\r", lRetVal);
    }

    //
    // configure the socket as TLS1.2
    //
    lRetVal = sl_SetSockOpt(iSockID, SL_SOL_SOCKET, SL_SO_SECMETHOD, &ucMethod,\
                               sizeof(ucMethod));
    if(lRetVal < 0) {
        return printErrConvenience("Device couldn't set socket options \n\r", lRetVal);
    }
    //
    //configure the socket as ECDHE RSA WITH AES256 CBC SHA
    //
    lRetVal = sl_SetSockOpt(iSockID, SL_SOL_SOCKET, SL_SO_SECURE_MASK, &uiCipher,\
                           sizeof(uiCipher));
    if(lRetVal < 0) {
        return printErrConvenience("Device couldn't set socket options \n\r", lRetVal);
    }



/////////////////////////////////
// START: COMMENT THIS OUT IF DISABLING SERVER VERIFICATION
    //
    //configure the socket with CA certificate - for server verification
    //
    lRetVal = sl_SetSockOpt(iSockID, SL_SOL_SOCKET, \
                           SL_SO_SECURE_FILES_CA_FILE_NAME, \
                           SL_SSL_CA_CERT, \
                           strlen(SL_SSL_CA_CERT));

    if(lRetVal < 0) {
        return printErrConvenience("Device couldn't set socket options \n\r", lRetVal);
    }
// END: COMMENT THIS OUT IF DISABLING SERVER VERIFICATION
/////////////////////////////////


    //configure the socket with Client Certificate - for server verification
    //
    lRetVal = sl_SetSockOpt(iSockID, SL_SOL_SOCKET, \
                SL_SO_SECURE_FILES_CERTIFICATE_FILE_NAME, \
                                    SL_SSL_CLIENT, \
                           strlen(SL_SSL_CLIENT));

    if(lRetVal < 0) {
        return printErrConvenience("Device couldn't set socket options \n\r", lRetVal);
    }

    //configure the socket with Private Key - for server verification
    //
    lRetVal = sl_SetSockOpt(iSockID, SL_SOL_SOCKET, \
            SL_SO_SECURE_FILES_PRIVATE_KEY_FILE_NAME, \
            SL_SSL_PRIVATE, \
                           strlen(SL_SSL_PRIVATE));

    if(lRetVal < 0) {
        return printErrConvenience("Device couldn't set socket options \n\r", lRetVal);
    }


    /* connect to the peer device - Google server */
    lRetVal = sl_Connect(iSockID, ( SlSockAddr_t *)&Addr, iAddrSize);

    if(lRetVal >= 0) {
        Report("Device has connected to the website:");
        Report(SERVER_NAME);
        Report("\n\r");
    }
    else if(lRetVal == SL_ESECSNOVERIFY) {
        Report("Device has connected to the website (UNVERIFIED):");
        Report(SERVER_NAME);
        Report("\n\r");
    }
    else if(lRetVal < 0) {
        Report("Device couldn't connect to server:");
        Report(SERVER_NAME);
        Report("\n\r");
        return printErrConvenience("Device couldn't connect to server \n\r", lRetVal);
    }

    GPIO_IF_LedOff(MCU_RED_LED_GPIO);
    GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
    return iSockID;
}



int connectToAccessPoint() {
    long lRetVal = -1;
    GPIO_IF_LedConfigure(LED1|LED3);

    GPIO_IF_LedOff(MCU_RED_LED_GPIO);
    GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);

    lRetVal = InitializeAppVariables();
    ASSERT_ON_ERROR(lRetVal);

    //
    // Following function configure the device to default state by cleaning
    // the persistent settings stored in NVMEM (viz. connection profiles &
    // policies, power policy etc)
    //
    // Applications may choose to skip this step if the developer is sure
    // that the device is in its default state at start of applicaton
    //
    // Note that all profiles and persistent settings that were done on the
    // device will be lost
    //
    lRetVal = ConfigureSimpleLinkToDefaultState();
    if(lRetVal < 0) {
      if (DEVICE_NOT_IN_STATION_MODE == lRetVal)
          Report("Failed to configure the device in its default state \n\r");

      return lRetVal;
    }

    Report("Device is configured in default state \n\r");

    CLR_STATUS_BIT_ALL(g_ulStatus);

    ///
    // Assumption is that the device is configured in station mode already
    // and it is in its default state
    //
    Report("Opening sl_start\n\r");
    lRetVal = sl_Start(0, 0, 0);
    if (lRetVal < 0 || ROLE_STA != lRetVal) {
        Report("Failed to start the device \n\r");
        return lRetVal;
    }

    Report("Device started as STATION \n\r");

    //
    //Connecting to WLAN AP
    //
    lRetVal = WlanConnect();
    if(lRetVal < 0) {
        Report("Failed to establish connection w/ an AP \n\r");
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        return lRetVal;
    }

    Report("Connection established w/ AP and IP is aquired \n\r");
    return 0;
}
//*****************************************************************************
//                      LOCAL FUNCTION DEFINITIONS
//*****************************************************************************

//void UARTIntHandler(void) // UART interrupt handler
//{
//    Report("In interrupt");
//    UARTIntClear(UARTA0_BASE, UART_INT_RX | UART_INT_RT | UART_INT_TX);
//
//    unsigned long intStatus = UARTIntStatus(UARTA0_BASE, true);
//
//    while (UARTCharsAvail(UARTA0_BASE)) {
//        unsigned char character = MAP_UARTCharGet(UARTA0_BASE);
//        if (character == '\0'){
//            long lRetVal = -1;
//            //Connect the CC3200 to the local access point
//            lRetVal = connectToAccessPoint();
//            //Set time so that encryption can be used
//            lRetVal = set_time();
//            if(lRetVal < 0) {
//                Report("Unable to set time in the device");
//                LOOP_FOREVER();
//            }
//            //Connect to the website with TLS encryption
//            lRetVal = tls_connect();
//            if(lRetVal < 0) {
//                ERR_PRINT(lRetVal);
//            }
//            http_post(lRetVal);
//            sl_Stop(SL_STOP_TIMEOUT);
//        } // terminator
//        else {
//            received[receive] = character;
//            receive++;
//        }
//
//    }
//}

char* decodeInput(void) {

    if (!strcmp(ZERO, input) || !strcmp(ONE, input) || !strcmp(MUTE, input) || !strcmp(LAST, input))
        multiPress = 0;
    else
        multiPress = 1;

    if (strcmp(prevButton, input) != 0 && multiPress && firstPress == 0 && prevPress) {
        messageIndex++;
        buttonIndex = 0;
    }

    appliedColors[messageIndex] = colors[colorIndex % 5];
  //  printf("%s\n",input);


    if(strcmp(ZERO, input) == 0){
        prevPress = 0;
        strcat(message, " ");
        messageIndex++;
        spaceUsed = 1;
        pause = 1;
        printf("space\n");

    }
    else if (strcmp(ONE, input) == 0){
        colorChange = 1;
        prevPress = 0;

        colorIndex++;
        printf("Color Changed\n");
    }
    else if (strcmp(TWO, input) == 0){
        prevPress = 1;

        buttonPressed = 1;
        message[messageIndex] = button2[buttonIndex%3];
        buttonIndex++;
        printf("2\n");
    }
    else if (strcmp(THREE, input) == 0){
        prevPress = 1;

        buttonPressed = 1;

        message[messageIndex] = button3[buttonIndex%3];
        buttonIndex++;
        printf("3\n");
    }
    else if (strcmp(FOUR, input) == 0){
        prevPress = 1;

        buttonPressed = 1;

     //   message[messageIndex] = button4[buttonIndex%3];
        buttonIndex++;
        left = 1;
        printf("4\n");
    }
    else if (strcmp(FIVE, input) == 0) {
        prevPress = 1;

        buttonPressed = 1;

        message[messageIndex] = button5[buttonIndex%3];
        buttonIndex++;
        printf("5\n");
    }
    else if (strcmp(SIX, input) == 0){
        prevPress = 1;

        buttonPressed = 1;

        message[messageIndex] = button6[buttonIndex%3];
        buttonIndex++;
        right = 1;

        printf("6\n");
    }
    else if (strcmp(SEVEN, input) == 0){
        prevPress = 1;

        buttonPressed = 1;

        message[messageIndex] = button7[buttonIndex%4];
        buttonIndex++;
        printf("7\n");
    }
    else if (strcmp(EIGHT, input) == 0){
        prevPress = 1;

        buttonPressed = 1;

        message[messageIndex] = button8[buttonIndex%3];
        buttonIndex++;
        start = 1;
        printf("8\n");
    }
    else if (strcmp(NINE, input) == 0) {
        prevPress = 1;

        buttonPressed = 1;

        message[messageIndex] = button9[buttonIndex%4];
        buttonIndex++;
        printf("9\n");

        return input;
    }
    else if (strcmp(MUTE, input) == 0) {
        prevPress = 0;

        delete = 1;

        messageIndex--;
        message[messageIndex] = '\0';
        printf("m\n");
    }
    else if (strcmp(LAST, input) == 0) {
        prevPress = 0;

        message[messageIndex] = '\0';
        sendFlag = 1;
        printf("l\n");
    }
    else
        return NULL;


    strcpy(prevButton, input); // for multipress
    firstPress = 0;
    return NULL;
}



void getData(void)
{
    if (interval >= 3) // 1.64 > ~3 times 0.5
        strcat(input, "1");
    else
        strcat(input, "0");

    bitlen++;

    if (bitlen == 32) // end of signal
        decodeInput();

}

void TimerA0IntHandler(void) {

    Timer_IF_InterruptClear(TIMERA0_BASE);
    interval++;
}





//*****************************************************************************
//
//! The interrupt handler for each rising edge from IR_IN
//!
//! \param  None
//!
//! \return none
//
//*****************************************************************************
static void GPIOA2IntHandler(void) {
    unsigned long ulStatus;
    long read;

    ulStatus = MAP_GPIOIntStatus (IR_IN.port, true);
    MAP_GPIOIntClear(IR_IN.port, ulStatus);     // clear interrupts

    read = GPIOPinRead(IR_IN.port, IR_IN.pin);


    if (read == 0x2) {  //rising edge
        interval = 0;
        MAP_TimerEnable(TIMERA0_BASE, TIMER_A);

    } else if (read == 0) {  //falling edge

        if (resetFlag == 1) {
                getData();
        }

        if (interval >= 8) { //long signal in beginning, singifies a reset
                input[0] = '\0';
                resetFlag = 1;
                bitlen = 0;
        }
    }



}

void UartInit() { // setting peripheral
  MAP_UARTConfigSetExpClk(
      UARTA1_BASE, MAP_PRCMPeripheralClockGet(PRCM_UARTA1), UART_BAUD_RATE,
      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

  //MAP_UARTIntRegister(UARTA1_BASE, UARTIntHandler);
  MAP_UARTIntEnable(UARTA1_BASE,  UART_INT_RX | UART_INT_RT | UART_INT_TX);

  unsigned long uartStatus = UARTIntStatus(UARTA1_BASE, false);
  UARTIntClear(UARTA1_BASE, uartStatus);

  MAP_UARTDMAEnable(UARTA1_BASE, UART_DMA_TX);
}

static int http_post(int iTLSSockID){
    for (i = 0; i < messageIndex + 1; i++)
    {
        //MAP_UARTCharPut(CONSOLE, message[i]);
        //Report("Here");
    }
    char acSendBuff[512];
    char acRecvbuff[1460];
    char cCLLength[200];
    char* pcBufHeaders;
    int lRetVal = 0;
    char* DATA = "";
    // creating post header
    strcat(DATA, PART1);
    strcat(DATA, message);
    strcat(DATA, PART2);
    Report(DATA);
    //char* DATA2 = "{\"state\": {\r\n\"desired\" : {\r\n\"var\" : \"EEC172 Wifi not work!\"\r\n}}}\r\n\r\n";
    pcBufHeaders = acSendBuff;
    strcpy(pcBufHeaders, POSTHEADER);
    pcBufHeaders += strlen(POSTHEADER);
    strcpy(pcBufHeaders, HOSTHEADER);
    pcBufHeaders += strlen(HOSTHEADER);
    strcpy(pcBufHeaders, CHEADER);
    pcBufHeaders += strlen(CHEADER);
    strcpy(pcBufHeaders, "\r\n\r\n");

    int dataLength = strlen(DATA);

    strcpy(pcBufHeaders, CTHEADER);
    pcBufHeaders += strlen(CTHEADER);
    strcpy(pcBufHeaders, CLHEADER1);

    pcBufHeaders += strlen(CLHEADER1);
    sprintf(cCLLength, "%d", dataLength);

    strcpy(pcBufHeaders, cCLLength);
    pcBufHeaders += strlen(cCLLength);
    strcpy(pcBufHeaders, CLHEADER2);
    pcBufHeaders += strlen(CLHEADER2);


    strcpy(pcBufHeaders, DATA);
    pcBufHeaders += strlen(DATA);

    int testDataLength = strlen(pcBufHeaders);

    Report(acSendBuff);


    //
    // Send the packet to the server */
    //
    lRetVal = sl_Send(iTLSSockID, acSendBuff, strlen(acSendBuff), 0);
    if(lRetVal < 0) {
        Report("POST failed. Error Number: %i\n\r",lRetVal);
        sl_Close(iTLSSockID);
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        return lRetVal;
    }
    lRetVal = sl_Recv(iTLSSockID, &acRecvbuff[0], sizeof(acRecvbuff), 0);
    if(lRetVal < 0) {
        Report("Received failed. Error Number: %i\n\r",lRetVal);
        //sl_Close(iSSLSockID);
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
           return lRetVal;
    }
    else {
        acRecvbuff[lRetVal+1] = '\0';
        Report(acRecvbuff);
        Report("\n\r\n\r");
    }

    return 0;
}
//****************************************************************************
//
//! Main function
//!
//! \param none
//!
//!
//! \return None.
//
//****************************************************************************
int main() {
    unsigned long ulStatus;
    long lRetVal = -1;

    BoardInit();

    PinMuxConfig();

    InitTerm();
    ClearTerm();
    Report("My terminal works!");
    I2C_IF_Open(I2C_MASTER_MODE_FST);

    MAP_GPIOIntRegister(IR_IN.port, GPIOA2IntHandler);

    //
    // Configure rising edge interrupt on IR_IN
    //
    MAP_GPIOIntTypeSet(IR_IN.port, IR_IN.pin, GPIO_BOTH_EDGES);

    ulStatus = MAP_GPIOIntStatus(IR_IN.port, false);
    MAP_GPIOIntClear(IR_IN.port, ulStatus);         // clear interrupts

    // Enable GPIO Interrupt and UART Receive interrupt
    MAP_GPIOIntEnable(IR_IN.port, IR_IN.pin);
    MAP_PRCMPeripheralClkEnable(PRCM_GSPI,PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralReset(PRCM_GSPI);

    MAP_SPIReset(GSPI_BASE);

    //
    // Configure SPI interface
    //
    MAP_SPIConfigSetExpClk(GSPI_BASE,MAP_PRCMPeripheralClockGet(PRCM_GSPI),
    SPI_IF_BIT_RATE,SPI_MODE_MASTER,SPI_SUB_MODE_0,
    (SPI_SW_CTRL_CS |
    SPI_4PIN_MODE |
    SPI_TURBO_OFF |
    SPI_CS_ACTIVEHIGH |
    SPI_WL_8));

    //
    // Enable SPI for communication
    //
    MAP_SPIEnable(GSPI_BASE);



    Timer_IF_Init(PRCM_TIMERA0, TIMERA0_BASE, TIMER_CFG_PERIODIC_UP, TIMER_A, 0);
    Timer_IF_IntSetup(TIMERA0_BASE, TIMER_A, TimerA0IntHandler);
    Timer_IF_Stop(TIMERA0_BASE, TIMER_A);
    MAP_TimerLoadSet(TIMERA0_BASE, TIMER_A, 40000);  //0.5ms

    Adafruit_Init();
    setCursor(0,0);
    setTextSize(2);
    fillScreen(BLUE);
    setTextColor(colors[colorIndex % 5], BLACK);

    // http_post(lRetVal);

    //sl_Stop(SL_STOP_TIMEOUT);

    initGame();



    unsigned char ucDevAddr = 0x18, xOffset = 0x3, yOffset = 0x5, ucRdLen = 1; //hardcoded values from earlier part of lab
    unsigned char XaucRdDataBuf; // to find tilt in y
    unsigned char YaucRdDataBuf; // to find tilt in x

while(!buttonPressed){}
    int pressedAgain =0;

    while (1) {
        while(pause == 1){
            if(start ==1){
                pause =0;
                start = 0;
                break;
            }
        }
        I2C_IF_Write(ucDevAddr, &xOffset, 1, 0);
        I2C_IF_Read(ucDevAddr, &XaucRdDataBuf, ucRdLen);
        dx = (int) ((signed char) XaucRdDataBuf);
        //fillScreen(BLACK); // Clear screen for the next frame
        handlePaddleMovement(dx);
        drawPaddle();
        drawBall();
        drawBricks();
        updateGame();
        if(gameoverbreak == 1){
            count++;
            sendFlag = 1;
            gameoverbreak = 0;
            fillScreen(BLUE);
            MAP_UtilsDelay(80000);
            initGame();
        }
        if (sendFlag == 1) {
            long lRetVal = -1;
            //Connect the CC3200 to the local access point
            lRetVal = connectToAccessPoint();
            //Set time so that encryption can be used
            lRetVal = set_time();
            if(lRetVal < 0) {
                Report("Unable to set time in the device");
                LOOP_FOREVER();
            }
            //Connect to the website with TLS encryption
            lRetVal = tls_connect();
            if(lRetVal < 0) {
                ERR_PRINT(lRetVal);
            }
            char str[12];               // String to store the converted integer, large enough for any 32-bit integer, plus the null terminator

            // Convert integer to string
            strcat(message,"");
            sprintf(str, "%d", count);
            strcat(message,str);
            http_post(lRetVal);
            sl_Stop(SL_STOP_TIMEOUT);
            sendFlag = 0;
        }

        // Delay to control frame rate
        // Delay function depends on your platform, adjust accordingly

    }

    fillScreen(BLUE);

    /*
    while (1) {
        if (delete) {
            delete = 0;
            fillScreen(BLACK);
            setCursor(0,0);

            int d;
            for (d = 0; d < messageIndex + 1; d++) { // printing with set colors
                setTextColor(appliedColors[d], BLACK);
                char letter = message[d];
                Outstr(&letter);
            }
            printf("Character Deleted\n");
        }


        while (buttonPressed) {
            buttonPressed = 0;
            int pressedAgain = 0;

            // Space key pressed, need to refresh screen to print properly
            if (spaceUsed || colorChange) {
                spaceUsed = 0;
                fillScreen(BLACK);
                setCursor(0,0);
            }
            fillScreen(BLACK);
            setCursor(0,0);

            int d;
            for (d = 0; d < messageIndex + 1; d++) { //printing using the changed colors
                setTextColor(appliedColors[d], BLACK);
                char letter = message[d];
                Outstr(&letter);
            }
            int c = 0;


            for (c = 0; c < 1000; c++) {
                if (buttonPressed) {
                    fillScreen(BLACK);
                    setCursor(0,0);
                    pressedAgain = 1;
                    break;
                }
            }
            if (pressedAgain) {
                continue;
            }

            printf("Character Locked In\n"); // signifier that time has elapsed
            messageIndex++;
            buttonIndex = 0;
        }

        if (sendFlag == 1) {
            for (i = 0; i < messageIndex + 1; i++)
                        {
                            MAP_UARTCharPut(CONSOLE, message[i]);
                            printf("%c", message[i]);
                        }
            fillRect(0, 0, 128, 64, BLACK); // upper half
            long lRetVal = -1;
            //Connect the CC3200 to the local access point
            lRetVal = connectToAccessPoint();
            //Set time so that encryption can be used
            lRetVal = set_time();
            if(lRetVal < 0) {
                Report("Unable to set time in the device");
                LOOP_FOREVER();
            }
            //Connect to the website with TLS encryption
            lRetVal = tls_connect();
            if(lRetVal < 0) {
                ERR_PRINT(lRetVal);
            }
            http_post(lRetVal);
            sl_Stop(SL_STOP_TIMEOUT);
            sendFlag = 0;
        }

    }
    */
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************



