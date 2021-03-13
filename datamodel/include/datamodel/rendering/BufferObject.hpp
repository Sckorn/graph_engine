#ifndef RCBE_ENGINE_BUFFEROBJECT_HPP
#define RCBE_ENGINE_BUFFEROBJECT_HPP

#include <vector>
#include <variant>
#include <optional>

#include <GL/gl.h>

#include <rcbe-engine/core/gl_extensions.hpp>

#include <rcbe-engine/datamodel/geometry/Mesh.hpp>

namespace rcbe::rendering {

class VertexBufferObject {
public:

    using ValueType = GLfloat;
    using StorageType = std::vector<ValueType>;
    using RawDataType = const ValueType *;

    class VertexArrayObject {
    public:
        VertexArrayObject();
        ~VertexArrayObject();

        VertexArrayObject(const VertexArrayObject &other) = default;
        VertexArrayObject &operator=(const VertexArrayObject &other) = default;

        VertexArrayObject(VertexArrayObject &&other) = default;
        VertexArrayObject &operator=(VertexArrayObject &&other) = default;

        void bind() const;
        void undbind() const;
        void setData(const StorageType &vertices) const;
    private:
        unsigned int id_ = 0;
        mutable bool binded_ = false;
    };

    explicit VertexBufferObject(const std::vector<rcbe::geometry::Mesh>& meshes, bool use_vao = false);

    ~VertexBufferObject();

    void enableState() const;
    void disableState() const;

    void bind() const;
    void unbind() const;

    [[nodiscard]] size_t size() const noexcept;
    [[nodiscard]] size_t sourceSize() const noexcept;
    [[nodiscard]] size_t sizeBytes() const noexcept;

    [[nodiscard]] size_t vertsByteSize() const noexcept;
    [[nodiscard]] size_t normsByteSize() const noexcept;
    [[nodiscard]] size_t colorsByteSize() const noexcept;

    [[nodiscard]] RawDataType verticesData() const noexcept;
    [[nodiscard]] RawDataType normalsData() const noexcept;
    [[nodiscard]] RawDataType colorsData() const noexcept;

    [[nodiscard]] const std::vector<size_t>& offsets() const noexcept;
    [[nodiscard]] const VertexArrayObject &vao() const;

private:
    bool normals_intact_ = false;

    GLuint id_ = 0;

    size_t source_size_; // since size of all filled containers (i.e. vertices, normals, colors) should be the same we keep only one size
    size_t buffer_size_bytes_;

    size_t vertices_byte_size_;
    size_t normals_byte_size_;
    size_t colors_byte_size_;

    StorageType vertices_;
    StorageType normals_;
    StorageType colors_;

    std::vector<size_t> vertices_offset_;

    std::optional<VertexArrayObject> vao_ {};
};

class IndexBufferObject {
public:
    using ValueType = GLuint;
    using StorageType = std::vector<ValueType>;
    using RawDataType = const ValueType *;

    class ElementBufferObject {
    public:
        ElementBufferObject();
        ~ElementBufferObject();

        void bind() const;
        void unbind() const;
        void setData(const StorageType &indices) const;

    private:
        unsigned int id_ = 0;
        mutable bool binded_ = false;
    };

    IndexBufferObject() = delete;
    ~IndexBufferObject();

    IndexBufferObject(const std::vector<rcbe::geometry::Mesh>& meshes, const VertexBufferObject& vbo, bool use_ebo = false);

    void bind() const;
    void unbind() const;

    [[nodiscard]] size_t size() const noexcept;

    [[nodiscard]] RawDataType data() const noexcept;

    [[nodiscard]] const ElementBufferObject &ebo() const;

private:
    GLuint id_;
    std::vector<size_t> vertices_offset_;
    StorageType indices_;

    std::optional<ElementBufferObject> ebo_ = {};
};

}

#endif //RCBE_ENGINE_BUFFEROBJECT_HPP
