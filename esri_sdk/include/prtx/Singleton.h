/*
  COPYRIGHT (c) 2012-2020 Esri R&D Center Zurich
  TRADE SECRETS: ESRI PROPRIETARY AND CONFIDENTIAL
  Unpublished material - all rights reserved under the
  Copyright Laws of the United States and applicable international
  laws, treaties, and conventions.

  For additional information, contact:
  Environmental Systems Research Institute, Inc.
  Attn: Contracts and Legal Services Department
  380 New York Street
  Redlands, California, 92373
  USA

  email: contracts@esri.com
*/

#ifndef PRTX_SINGLETON_H_
#define PRTX_SINGLETON_H_

namespace prtx {


/**
 * Singleton helper template typically used by subclasses of ExtensionFactory.
 * This class uses the static mix-in pattern (a.k.a. CRTP) to let subclasses implement their
 * own singleton creation in createInstance. See EncoderFactory for a usage example.
 *
 * \sa EncoderFactory
 */
template<typename S> class Singleton {
public:
	static S* instance() {
		if (!mInstance)
			mInstance = S::createInstance();
		return mInstance;
	}

	/**
	 * Subclasses must implement this static member function to create the factory singletons.
	 *
	 * @return The single instance of factory class S.
	 */
	static S* createInstance();

protected:
	Singleton() { }
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	~Singleton() { }

private:
	static S* mInstance;
};

template<typename S> S* Singleton<S>::mInstance = 0;


} // namespace prtx


#endif /* PRTX_SINGLETON_H_ */
