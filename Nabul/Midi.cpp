// Midi.cpp

#include "stdafx.h"
#include "Midi.h"

CMidi::CMidi()
{

	m_nDevices = midiOutGetNumDevs();
	m_bOpened = m_bPaused = m_bPlaying = FALSE;
	m_wDeviceID = 0;

}

CMidi::~CMidi()
{

	Close();

}

int CMidi::DeviceCount( void )
{

	return( m_nDevices );

}

BOOL CMidi::Open( const char *lpszFilename )
{

	if( !m_bOpened ){

		// Initialize member variables.
		m_bPaused = m_bPlaying = FALSE;
		m_wDeviceID = 0;

		// See if the file exists.
		CFileStatus Status;
		if( !CFile::GetStatus( lpszFilename, Status ) )
			return( FALSE );

		// Open the device.
		MCI_OPEN_PARMS OpenParms;
		OpenParms.lpstrDeviceType = (LPCSTR) MCI_DEVTYPE_SEQUENCER;
		OpenParms.lpstrElementName = (LPCSTR) lpszFilename;
		OpenParms.wDeviceID = 0;
		if( mciSendCommand( NULL, MCI_OPEN, MCI_WAIT | MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID | MCI_OPEN_ELEMENT, (DWORD)(LPVOID) &OpenParms ) )
			return( FALSE );
		m_wDeviceID = OpenParms.wDeviceID;
		m_bOpened = TRUE;

		// Set the time format to milliseconds.
		MCI_SET_PARMS SetParms;
		SetParms.dwTimeFormat = MCI_FORMAT_MILLISECONDS;
		if( mciSendCommand( m_wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD)(LPVOID) &SetParms ) ){
			Close();
			return( FALSE );
			}
		mciSendCommand( m_wDeviceID, MCI_SEEK, MCI_SEEK_TO_START, NULL );
		return( TRUE );
		}

	return( FALSE );

}

BOOL CMidi::Close( void )
{

	if( m_bOpened ){

		// Stop the playing and close the device.
		if( m_bPlaying || m_bPaused )
			mciSendCommand( m_wDeviceID, MCI_STOP, NULL, NULL );
		mciSendCommand( m_wDeviceID, MCI_CLOSE, NULL, NULL );

		// Clear member variables.
		m_bOpened = m_bPaused = m_bPlaying = FALSE;
		m_wDeviceID = 0;

		return( TRUE );
		}

	return( FALSE );

}

BOOL CMidi::Play( void )
{

	if( m_bOpened ){

		// Issue the 'play' command.
		MCI_PLAY_PARMS PlayParms;
		PlayParms.dwCallback = NULL;
		PlayParms.dwFrom = ( ( GetMinutes() * 60 ) + GetSeconds () ) * 1000;
		if( mciSendCommand( m_wDeviceID, MCI_PLAY, MCI_FROM, (DWORD)(LPVOID) &PlayParms ) )
			return( FALSE );

		// Set our class members so we know that
		// we're currently playing.
		m_bPlaying = TRUE;
		m_bPaused = FALSE;

		return( TRUE );
		}

	return( FALSE );

}

BOOL CMidi::Stop( void )
{

	if( m_bOpened && m_bPlaying ){
		// Issue the 'stop' command.
		mciSendCommand( m_wDeviceID, MCI_STOP, NULL, NULL );
		// Issue the command that seeks back to the start.
		mciSendCommand( m_wDeviceID, MCI_SEEK, MCI_SEEK_TO_START, NULL );
		m_bPaused = m_bPlaying = FALSE;

		return( TRUE );
		}

	return( FALSE );

}

BOOL CMidi::Pause( void )
{

	if( m_bOpened && m_bPlaying ){

		// Pause the CD.
		MCI_PLAY_PARMS PlayParms;
		if( mciSendCommand( m_wDeviceID, MCI_PAUSE, 0, (DWORD)(LPVOID) &PlayParms ) )
			return( FALSE );

		m_bPaused = TRUE;

		return( TRUE );
		}

	return( FALSE );

}

BOOL CMidi::IsPlaying( void )
{

	if( m_bOpened ){

		// Issue the 'status' command.
		MCI_STATUS_PARMS StatusParms;
		StatusParms.dwItem = MCI_STATUS_MODE;
		if( mciSendCommand( m_wDeviceID, MCI_STATUS, MCI_WAIT | MCI_STATUS_ITEM, (DWORD)(LPVOID) &StatusParms ) )
			return( FALSE );

		if( StatusParms.dwReturn == MCI_MODE_PLAY ){
			m_bPlaying = TRUE;
			m_bPaused = FALSE;
			return( TRUE );
			}
		else if( StatusParms.dwReturn == MCI_MODE_PAUSE ){
			m_bPlaying = TRUE;
			m_bPaused = FALSE;
			return( TRUE );
			}
		else{
			m_bPlaying = FALSE;
			m_bPaused = FALSE;
			return( FALSE );
			}
		}

	return( FALSE );

}

BOOL CMidi::GetLength( int *lpnMinutes, int *lpnSeconds )
{

	// Set minutes and seconds to -1 in
	// case there's an error.
	*lpnMinutes = -1;
	*lpnSeconds = -1;

	if( m_bOpened ){

		// Issue the 'status/length' command.
		MCI_STATUS_PARMS StatusParms;
		StatusParms.dwItem = MCI_STATUS_LENGTH;
		if( mciSendCommand( m_wDeviceID, MCI_STATUS, MCI_WAIT | MCI_STATUS_ITEM, (DWORD)(LPVOID) &StatusParms ) )
			return( FALSE );

		// Store the values in *lpnMinutes and *lpnSeconds.
		*lpnMinutes = ( StatusParms.dwReturn / 1000 ) / 60;
		*lpnSeconds = ( StatusParms.dwReturn / 1000 ) % 60;

		return( TRUE );

		}

	return( FALSE );

}

int CMidi::GetMinutes( void )
{

	if( m_bOpened ){

		// Get the current position.
		MCI_STATUS_PARMS StatusParms;
		StatusParms.dwItem = MCI_STATUS_POSITION;
		if( mciSendCommand( m_wDeviceID, MCI_STATUS, MCI_WAIT | MCI_STATUS_ITEM, (DWORD)(LPVOID) &StatusParms ) )
			return( -1 );

		return( (int) ( ( StatusParms.dwReturn / 1000 ) / 60 ) );
		}

	return( -1 );

}

int CMidi::GetSeconds( void )
{

	if( m_bOpened ){

		// Get the current position.
		MCI_STATUS_PARMS StatusParms;
		StatusParms.dwItem = MCI_STATUS_POSITION;
		if( mciSendCommand( m_wDeviceID, MCI_STATUS, MCI_WAIT | MCI_STATUS_ITEM, (DWORD)(LPVOID) &StatusParms ) )
			return( -1 );

		return( (int) ( ( StatusParms.dwReturn / 1000 ) % 60 ) );
		}

	return( -1 );

}

BOOL CMidi::SeekTo( int nMinute, int nSecond )
{

	if( m_bOpened ){

		// Set dwTo to the correct value.
		MCI_SEEK_PARMS SeekParms;
		SeekParms.dwTo = ( nMinute * 60 + nSecond ) * 1000;

		// Pause if we're not paused.
		if( m_bPlaying && !m_bPaused )
			mciSendCommand( m_wDeviceID, MCI_PAUSE, 0, NULL );

		// Issue the 'seek' command.
		if( mciSendCommand( m_wDeviceID, MCI_SEEK, MCI_TO | MCI_WAIT, (DWORD)(LPVOID) &SeekParms ) )
			return( FALSE );

		// Restart the audio.
		if( m_bPlaying && !m_bPaused )
			return( Play() );

		}

	return( FALSE );

}

BOOL CMidi::SkipForward( int nSeconds )
{

	if( m_bOpened ){

		// Get the current position.
		MCI_STATUS_PARMS StatusParms;
		DWORD dwPos;
		StatusParms.dwItem = MCI_STATUS_POSITION;
		mciSendCommand( m_wDeviceID, MCI_STATUS, MCI_WAIT | MCI_STATUS_ITEM, (DWORD)(LPVOID) &StatusParms );
		dwPos = StatusParms.dwReturn;

		// Skip forward n milliseconds
		dwPos += (DWORD) nSeconds * 1000;

		// Pause and seek to.
		MCI_SEEK_PARMS SeekParms;
		SeekParms.dwTo = dwPos;
		if( m_bPlaying && !m_bPaused )
			mciSendCommand( m_wDeviceID, MCI_PAUSE, 0, NULL );
		mciSendCommand (m_wDeviceID, MCI_SEEK, MCI_TO | MCI_WAIT, (DWORD)(LPVOID) &SeekParms);

		// Restart the audio.
		if( m_bPlaying && !m_bPaused )
			return( Play() );

		}

	return( FALSE );

}

BOOL CMidi::SkipBack( int nSeconds )
{

	if( m_bOpened ){

		// Get the current position.
		MCI_STATUS_PARMS StatusParms;
		DWORD dwPos;
		StatusParms.dwItem = MCI_STATUS_POSITION;
		mciSendCommand( m_wDeviceID, MCI_STATUS, MCI_WAIT | MCI_STATUS_ITEM, (DWORD)(LPVOID) &StatusParms );
		dwPos = StatusParms.dwReturn;

		// Skip forward n milliseconds.
		dwPos -= (DWORD) nSeconds * 1000;

		// Pause and seek to.
		MCI_SEEK_PARMS SeekParms;
		SeekParms.dwTo = dwPos;
		if( m_bPlaying && !m_bPaused )
			mciSendCommand( m_wDeviceID, MCI_PAUSE, 0, NULL );
		mciSendCommand( m_wDeviceID, MCI_SEEK, MCI_TO | MCI_WAIT, (DWORD)(LPVOID) &SeekParms );

		// Restart the audio.
		if( m_bPlaying && !m_bPaused )
			return( Play() );

		}

	return( FALSE );

}

int CMidi::GetTempo( void )
{

	if( m_bOpened ){

		// Issue the 'status/tempo' command.
		MCI_STATUS_PARMS StatusParms;
		StatusParms.dwItem = MCI_SEQ_STATUS_TEMPO;
		if( mciSendCommand( m_wDeviceID, MCI_STATUS, MCI_WAIT | MCI_STATUS_ITEM, (DWORD)(LPVOID) &StatusParms ) )
			return( -1 );

		return( (int) StatusParms.dwReturn );
		}

	return( -1 );

}

BOOL CMidi::SetTempo( int nTempo )
{

	if( m_bOpened ){

		// Set the tempo.
		MCI_SEQ_SET_PARMS SeqSetParms;
		SeqSetParms.dwTempo = nTempo;
		mciSendCommand( m_wDeviceID, MCI_SET, MCI_WAIT | MCI_SEQ_SET_TEMPO, (DWORD)(LPVOID) &SeqSetParms );
		return( TRUE );
		}

	return( FALSE );

}

