#ifndef TOPAZ_RENDER_DEVICE_HPP
#define TOPAZ_RENDER_DEVICE_HPP
#include "gl/index_snippet.hpp"
#include <initializer_list>
#include <optional>

// Forward declares
namespace tz
{
	namespace gl
	{
		class IFrame;
		class ShaderProgram;
		class Object;
	}
}

namespace tz::render
{
	/**
     * \addtogroup tz_render Topaz Rendering Library (tz::render)
     * High-level interface for 3D and 2D hardware-accelerated graphics programming. Used in combination with the \ref tz_gl "Topaz Graphics Library".
     * @{
     */

	/**
	 * Represents and controls the conditions under which render-invocations will take place.
	 * 
	 * Devices will always render to the given frame, using the given program and drawing via the given object.
	 * Attempting to render when any of these are invalid will assert and invoke UB.
	 */
	class Device
	{
	public:
		/**
		 * Construct a Device with the given conditions.
		 * @param frame Frame-object to render into.
		 * @param prg Shader Program to bind and render with.
		 * @param object Object whose data will be used to render.
		 */
		Device(tz::gl::IFrame* frame, tz::gl::ShaderProgram* prg, tz::gl::Object* object);
		/**
		 * Retrieve the null-device. This device is invalid and should never be used to render.
		 * @return The null-device.
		 */
		static Device null_device();
		/**
		 * Query as to whether this device is equal to the null device.
		 * @return True if this device is the null-device. Otherwise false
		 */
		bool is_null() const;
		/**
		 * Refer subsequent render-invocations to the given ID handle corresponding to an index-buffer inside of the Object.
		 * 
		 * Precondition: ibo_id must be a valid index in the Object and refer to a valid index-buffer. Otherwise, this will assert and invoke UB.
		 * @param ibo_id Handle ID corresponding to the index-buffer which will be used in render invocations.
		 */
		void set_handle(std::size_t ibo_id);
		/**
		 * Provide a list of snippets of offsets into the index-buffer.
		 * 
		 * Note: After providing a snippet, future render-invocations will invoke multi-render.
		 * Precondition: snippet must contain index-ranges valid corresponding to the index-buffer which will be bound against this Device.
		 * @param snippet Snippet containing index-ranges used in MDI.
		 */
		void set_indices(tz::gl::IndexSnippetList indices);
		/**
		 * Invoke a render-invocation, making the Object emit a draw-call via the ibo_id set via this->set_handle.
		 * 
		 * Note: If an index-snippet was passed via this->set_snippet, then the Object will emit a multi-render call instead using the MDI draw command list generated by the snippet.
		 * Precondition: An ID handle must have been set via this->set_handle. Otherwise, this method will early-out and do nothing.
		 * Precondition: If a snippet was provided by this->set_snippet, it must contain index-ranges valid in the context of the ibo_id provided by this->set_handle earlier. Otherwise, this will assert and invoke UB.
		 */
		void render() const;
		/**
		 * Force the attached Frame to clear its backbuffer.
		 */
		void clear() const;
		/**
		 * Query as to whether the Device is ready to be used for rendering.
		 * A Device is ready if all of the following is true:
		 * - References a valid and complete IFrame (see tz::gl::IFrame::complete()).
		 * - References a valid and useable ShaderProgram (see tz::gl::ShaderProgram::usable()).
		 * - References a valid tz::gl::Object (Only check so far is nullness so a false-positive is possible).
		 * Note: A Device must be ready before it is rendered.
		 * @return True if ready. Otherwise false.
		 */
		bool ready() const;
		/// Deep-comparison
		bool operator==(const Device& rhs) const;
	private:
		void ensure_bound() const;
		/**
		 * Ensures that all indices specified by all snippets exist within the IBO.
		 */
		static bool sanity_check(const tz::gl::IndexSnippetList& indices, const tz::gl::IBO& ibo);

		tz::gl::IFrame* frame;
		tz::gl::ShaderProgram* program;
		tz::gl::Object* object;
		std::optional<std::size_t> ibo_id;
		tz::gl::IndexSnippetList snippets;
	};

	/**
	 * @}
	 */
}

#endif // TOPAZ_RENDER_DEVICE_HPP