#include <rcbe-engine/datamodel/rendering/BufferObject.hpp>

#include <boost/log/trivial.hpp>

namespace rcbe::rendering {

VertexBufferObject::VertexArrayObject::VertexArrayObject() {
    glGenVertexArrays(1, &id_);
}

VertexBufferObject::VertexArrayObject::~VertexArrayObject() {
    glDeleteVertexArrays(1, &id_);
}

void VertexBufferObject::VertexArrayObject::bind() const {
    //if (!binded_) {
        glBindVertexArray(id_);
    //    binded_ = true;
    //}
}

void VertexBufferObject::VertexArrayObject::undbind() const {
    //if (binded_) {
        glBindVertexArray(0);
    //    binded_ = false;
    //}
}

void VertexBufferObject::VertexArrayObject::setData(const StorageType &vertices) const {
    glVertexAttribPointer(
            0, 3, GL_FLOAT, false,
            vertices.size(), (void*)0
            );
    //glEnableVertexAttribArray(0);
}

VertexBufferObject::VertexBufferObject(const std::vector<rcbe::geometry::Mesh>& meshes, bool use_vao ) {
    // right now keeping it simple, it's not a VBO's business to validate meshes
    // they'll be validated by scene graph

    size_t normals_size = 0;
    source_size_ = std::accumulate(meshes.begin(), meshes.end(), 0, [&normals_size](auto sum, const auto &s) mutable {
        normals_size += s.normalsSize();
        return  sum + s.verticesSize();
    });

    normals_intact_ = source_size_ == normals_size;

    vertices_.reserve(source_size_ * 3);
    colors_.reserve(source_size_ * 3);

    if (!normals_intact_) {
        BOOST_LOG_TRIVIAL(warning) << "Normals are of wrong size!";
    } else {
        normals_.reserve(source_size_ * 3);
    }

    vertices_offset_.push_back(0);
    for (const auto& m : meshes) {
        vertices_offset_.push_back(m.verticesSize());

        const auto& vertices = m.vertices();
        const auto& normals = m.normals();
        const auto& color = m.color();

        for (size_t i = 0; i < vertices.size(); ++i) {
            const auto& v = vertices[i];

            vertices_.push_back(v.x());
            vertices_.push_back(v.y());
            vertices_.push_back(v.z());

            colors_.push_back(color.r());
            colors_.push_back(color.g());
            colors_.push_back(color.b());

            //if (normals_intact_) {
                const auto& n = normals[i];

                normals_.push_back(n.x());
                normals_.push_back(n.y());
                normals_.push_back(n.z());
            //}
        }
    }

    vertices_byte_size_ = (sizeof(decltype(vertices_)::value_type) * vertices_.size());
    normals_byte_size_ = (sizeof(decltype(normals_)::value_type) * normals_.size());
    colors_byte_size_ = (sizeof(decltype(colors_)::value_type) * colors_.size());

    buffer_size_bytes_ = vertices_byte_size_ + normals_byte_size_ + colors_byte_size_;

    if (use_vao)
        vao_ = VertexArrayObject();
    glGenBuffers(1, &id_);

    if (use_vao)
        vao_->bind();
    bind();

    glBufferData(GL_ARRAY_BUFFER, buffer_size_bytes_, 0, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices_byte_size_, vertices_.data());
    glBufferSubData(GL_ARRAY_BUFFER, vertices_byte_size_, normals_byte_size_, normals_.data());
    glBufferSubData(GL_ARRAY_BUFFER, vertices_byte_size_ + normals_byte_size_, colors_byte_size_, colors_.data());

    if (use_vao)
        vao_->setData(vertices_);

    unbind();
    if (use_vao)
        vao_->undbind();
}

VertexBufferObject::~VertexBufferObject() {
    glDeleteBuffers(1, &id_);
}

void VertexBufferObject::enableState() const {
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
}

void VertexBufferObject::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, id_);
}

void VertexBufferObject::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBufferObject::disableState() const {
    glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

size_t VertexBufferObject::size() const noexcept {
    return vertices_.size();
}

size_t VertexBufferObject::sourceSize() const noexcept {
    return source_size_;
}

size_t VertexBufferObject::sizeBytes() const noexcept {
    return buffer_size_bytes_;
}

size_t VertexBufferObject::vertsByteSize() const noexcept {
    return vertices_byte_size_;
}

size_t VertexBufferObject::normsByteSize() const noexcept {
    return normals_byte_size_;
}

size_t VertexBufferObject::colorsByteSize() const noexcept {
    return colors_byte_size_;
}

VertexBufferObject::RawDataType VertexBufferObject::verticesData() const noexcept {
    return vertices_.data();
}

VertexBufferObject::RawDataType VertexBufferObject::normalsData() const noexcept {
    return normals_.data();
}

VertexBufferObject::RawDataType VertexBufferObject::colorsData() const noexcept {
    return colors_.data();
}

const std::vector<size_t>& VertexBufferObject::offsets() const noexcept {
    return vertices_offset_;
}

const VertexBufferObject::VertexArrayObject &VertexBufferObject::vao() const {
    if (vao_)
        return *vao_;
    else
        throw std::runtime_error("VAO wasn't set, which most likely means use_vao flag wasn't supplied to ctor");
}

IndexBufferObject::ElementBufferObject::ElementBufferObject() {
    glGenBuffers(1, &id_);
}

IndexBufferObject::ElementBufferObject::~ElementBufferObject() {
    glDeleteBuffers(1, &id_);
}

void IndexBufferObject::ElementBufferObject::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
}

void IndexBufferObject::ElementBufferObject::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBufferObject::ElementBufferObject::setData(const StorageType &indices) const {
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(typename StorageType::value_type), indices.data(), GL_STATIC_DRAW);
}

IndexBufferObject::IndexBufferObject(const std::vector<rcbe::geometry::Mesh>& meshes, const VertexBufferObject& vbo, bool use_ebo) {
    auto itotal = std::accumulate(meshes.begin(), meshes.end(), 0, [](auto sum, const auto &entry) {
        return sum + entry.facetsSize();
    });

    vertices_offset_ = vbo.offsets();

    indices_.reserve(itotal * 3);

    size_t i = 0;
    for (auto &m : meshes)
    {
        const auto offset = vertices_offset_[i];
        for (const auto &f : m.facets())
        {
            indices_.push_back(f.indices[0] + offset);
            indices_.push_back(f.indices[1] + offset);
            indices_.push_back(f.indices[2] + offset);
        }
        ++i;
    }

    if (use_ebo)
        ebo_ = ElementBufferObject();
    else
        glGenBuffers(1, &id_);


    if (use_ebo)
        ebo_->bind();
    else
        bind();

    if (use_ebo)
        ebo_->setData(indices_);
    else
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(decltype(indices_)::value_type), &indices_[0], GL_STATIC_DRAW);

    if(use_ebo)
        ebo_->unbind();
    else
        unbind();
}

IndexBufferObject::~IndexBufferObject() {
    glDeleteBuffers(1, &id_);
}

void IndexBufferObject::bind() const {
    if (ebo_) {
        ebo_->bind();
        return;
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
}

void IndexBufferObject::unbind() const {
    if (ebo_) {
        ebo_->unbind();
        return;
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

size_t IndexBufferObject::size() const noexcept {
    return indices_.size();
}

IndexBufferObject::RawDataType IndexBufferObject::data() const noexcept {
    return indices_.data();
}

const IndexBufferObject::ElementBufferObject &IndexBufferObject::ebo() const {
    if (ebo_)
        return *ebo_;
    else
        throw std::runtime_error("EBO wasn't set, which most likely means use_ebo flag wasn't supplied to ctor");
}

}
