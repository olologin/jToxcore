/* JTox.java
 *
 *  Copyright (C) 2013 Tox project All Rights Reserved.
 *
 *  This file is part of jToxcore
 *
 *  jToxcore is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  jToxcore is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with jToxcore.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

package im.tox.jtoxcore;

/**
 * This is the main wrapper class for the tox library. It contains wrapper
 * methods for everything in the public API provided by tox.h
 * 
 * @author sonOfRa
 * 
 */
public class JTox {

	/**
	 * This field contains the pointer used in all native tox_ method calls.
	 */
	private long messengerPointer;

	/**
	 * Create a new instance of JTox with a given messengerPointer. Due to the
	 * nature of the native calls, this call is <i>unsafe<i>. Calling it with an
	 * arbitrary long that was not obtained from another JTox instance will
	 * result in undefined behavior when calling other methods on this instance.
	 * This method is mainly aimed at Android Developers to ease passing the
	 * JTox instance around between different activities.
	 * 
	 * @param messengerPointer
	 */
	public JTox(long messengerPointer) {
		this.messengerPointer = messengerPointer;
	}

	/**
	 * Native call to tox_new
	 * 
	 * @return the pointer to the messenger struct on success, 0 on failure
	 */
	private static native long tox_new();

	/**
	 * Creates a new instance of JTox and stores the pointer to the internal
	 * struct in messengerPointer. We are not using a constructor here in order
	 * to avoid partial objects being created when the native calls fail
	 * 
	 * @return a new JTox instance
	 * @throws ToxException
	 *             when the native call indicates an error
	 */
	public static JTox newTox() throws ToxException {
		long pointer = tox_new();

		if (pointer == 0) {
			throw new ToxException(ToxError.TOX_FAERR_UNKNOWN);
		} else {
			return new JTox(pointer);
		}
	}

	/**
	 * Native call to tox_addfriend
	 * 
	 * @param address
	 *            address of the friend
	 * @param data
	 *            optional message sent to friend
	 * @param length
	 *            length of the message sent to friend
	 * @return friend number on success, error code on failure
	 */
	private native int tox_addfriend(long messengerPointer, String address,
			String data);

	/**
	 * Use this method to add a friend.
	 * 
	 * @param address
	 *            the address of the friend you want to add
	 * @param data
	 *            an optional message you want to send to your friend
	 * @return the local number of the friend in your list
	 * @throws ToxException
	 *             in case an error code is returned by the native tox_addfriend
	 *             call
	 */
	public int addFriend(String address, String data) throws ToxException {
		int errcode = tox_addfriend(this.messengerPointer, address, data);

		if (errcode >= 0) {
			return errcode;
		} else {
			throw new ToxException(errcode);
		}
	}

	/**
	 * 
	 * @return the pointer to the internal messenger struct, as a long
	 */
	public long getPointer() {
		return this.messengerPointer;
	}
}
