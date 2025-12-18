#ifndef USERLIB_H
#define USERLIB_H

namespace userlib {
	namespace helperclass
	{
		class NonCopyable {
		protected:
			NonCopyable() = default;
			~NonCopyable() = default;
		private:
			NonCopyable(const NonCopyable&) = delete;
			NonCopyable& operator=(const NonCopyable&) = delete;
		};
	}
}


#endif // !USERLIB_H
