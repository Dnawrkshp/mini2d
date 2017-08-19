/*
 * Dialog.hpp
 *
 *  Created on: November 8th, 2016
 *  Author: Daniel Gerendasy
 */

#ifndef MINI2D_DIALOG_HPP_
#define MINI2D_DIALOG_HPP_

#include <sysutil/msg.h>                         // Message dialog functions/enums

#include <Mini2D/Mini.hpp>                       // Class definition

namespace Mini2D {

	class Dialog {
	public:
		msgButton DialogResponse;                // Response from msgDialogCallback (0 if no response)

		// Constructors
		Dialog(Mini * mini);
		virtual ~Dialog();

		/*
		 * Pop:
		 * 		Displays a message dialog
		 *
		 * type:
		 * 		Message dialog type to pass to msgDialogOpen2
		 * message:
		 * 		Message text
		 * wait:
		 * 		Whether or not to wait for the dialog to close before returning
		 * ms:
		 * 		Time (in milliseconds) to wait before automatically closing the dialog
		 *
		 * Return:
		 * 		The response from the message dialog if wait is true. 0 otherwise
		 */
		msgButton Pop(msgType type, const char * message, bool wait, unsigned int ms);
		msgButton Pop(msgType type, const char * message, bool wait);

		/*
		 * PopError:
		 * 		Displays an error dialog
		 *
		 * errorCode:
		 * 		Error code to display
		 * wait:
		 * 		Whether or not to wait for the dialog to close before returning
		 * ms:
		 * 		Time (in milliseconds) to wait before automatically closing the dialog
		 *
		 * Return:
		 * 		The response from the message dialog if wait is true. 0 otherwise
		 */
		msgButton PopError(unsigned int errorCode, bool wait, unsigned int ms);
		msgButton PopError(unsigned int errorCode, bool wait);

		/*
		 * PopSingleProgressBar:
		 * 		Displays a single progress bar dialog
		 *
		 * dialogMessage:
		 * 		Main dialog messaage
		 * progressMessage:
		 * 		Message to display next to the progress bar
		 * msAfterComplete: (optional)
		 * 		Time (in milliseconds) to wait before automatically closing the dialog AFTER the progress bar reaches 100%
		 */
		void PopSingleProgressBar(const char * dialogMessage, const char * progressMessage, unsigned int msAfterComplete = 0);

		/*
		 * PopDoubleProgressBar:
		 * 		Displays a single progress bar dialog
		 *
		 * dialogMessage:
		 * 		Main dialog messaage
		 * progressMessage1:
		 * 		Message to display next to the first progress bar
		 * progressMessage2:
		 * 		Message to display next to the second progress bar
		 * msAfterComplete: (optional)
		 * 		Time (in milliseconds) to wait before automatically closing the dialog AFTER the progress bar reaches 100%
		 */
		void PopDoubleProgressBar(const char * dialogMessage, const char * progressMessage1, const char * progressMessage2, unsigned int msAfterComplete = 0);

		/*
		 * ProgressBarIncrease:
		 * 		Increases the progress bar percent
		 *
		 * index:
		 * 		Increase the first or second progress bar
		 * percent:
		 * 		Percent to increase by
		 */
		void ProgressBarIncrease(bool index, unsigned int percent);

		/*
		 * ProgressBarSet:
		 * 		Set the progress bar percent
		 *
		 * index:
		 * 		Set the first or second progress bar
		 * percent:
		 * 		Percent to set to
		 */
		void ProgressBarSet(bool index, unsigned int percent);

		/*
		 * ProgressBarSetMessage:
		 * 		Set the progress bar message
		 *
		 * index:
		 * 		Set the first or second progress bar message
		 * progressMessage:
		 * 		Message
		 */
		void ProgressBarSetMessage(bool index, const char * progressMessage);

		/*
		 * ProgressBarReset:
		 * 		Reset the progress bar percent to 0%
		 *
		 * index:
		 * 		Reset the first or second progress bar
		 */
		void ProgressBarReset(bool index);

		/*
		 * ProgressBarClose:
		 * 		Close the dialog
		 */
		void ProgressBarClose();

	private:
		Mini * _mini;

		unsigned int _msAfterComplete;           // Time (in milliseconds) to wait before automatically closing the dialog AFTER the progress bar reaches 100%

		unsigned int _progressBarPercent[2];     // Current percent of progress bars

		// Check sysutil callback and flip frame
		void Flip(bool flip = 0);
	};

}

#endif /* MINI2D_DIALOG_HPP_ */
