//�Q�l:https://cpprefjp.github.io/lang/cpp20/coroutines.html

#include <iostream>
#include <coroutine>
#include <utility>

// �R���[�`�����p���C�u����: �W�F�l���[�^�^
struct generator {
    // �W�F�l���[�^�Ɋ֘A�t������Promise�^
    struct promise_type {
        // co_yield���Ŏw�肳���int�l��ێ�����ϐ�
        int value_;

        auto get_return_object()
        {
            // �R���[�`���ɕR�Â�Promise�I�u�W�F�N�g(*this)����
            // �W�F�l���[�^�^�̃R���[�`���߂�l�I�u�W�F�N�g�𐶐�
            return generator{ *this };
        };
        auto initial_suspend()
        {
            // �R���[�`���{�̏����̊J�n�O�ɖ������T�X�y���h
            return std::suspend_always{};
        }
        auto final_suspend() noexcept
        {
            // �R���[�`���{�̏����̏I����ɖ������T�X�y���h
            return std::suspend_always{};
        }
        auto yield_value(int v)
        {
            // co_yield���œn�����l��ێ����A�R���[�`���𖳏����T�X�y���h
            value_ = v;
            return std::suspend_always{};
        }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };
    // �W�F�l���[�^�Ɋ֘A�t������R���[�`���n���h���^
    using coro_handle = std::coroutine_handle<promise_type>;

    // �͈�for�\���T�|�[�g�p�C�e���[�^�^
    struct iterator {
        // �Ώۂ̃R���[�`���n���h��
        coro_handle coro_;
        // �ΏۃR���[�`���{�̏������I����������\���t���O
        bool done_;

        iterator& operator++()
        {
            // yield_value()�Œ��f�����R���[�`�����ĊJ����
            coro_.resume();
            // (co_yield���]���������̓R���[�`���{�̏����̏I���ɂ�萧�䂪�߂��Ă���)
            done_ = coro_.done();
            return *this;
        }
        bool operator!=(const iterator& rhs) const
        {
            return done_ != rhs.done_;
        }
        int operator*() const
        {
            // Promise�I�u�W�F�N�g���ێ����Ă���l��Ԃ�
            return coro_.promise().value_;
        }
    };

    ~generator()
    {
        if (coro_)
            coro_.destroy();
    }

    generator(generator const&) = delete;
    generator(generator&& rhs) noexcept
        : coro_(std::exchange(rhs.coro_, nullptr)) {}

    // �͈�for�\���T�|�[�g�p�̃����o�֐�
    iterator begin()
    {
        // initial_suspend()�Œ��f�����R���[�`�����ĊJ����
        coro_.resume();
        // (����co_yield���]���ɂ�萧�䂪�߂��Ă���)
        return { coro_, coro_.done() };
    }
    iterator end()
    {
        // �I�[�ʒu��\������ԕ��C�e���[�^
        return { {}, true };
    }

private:
    // Promise�I�u�W�F�N�g�o�R�ŃR���[�`���n���h�����擾����
    explicit generator(promise_type& p)
        : coro_(coro_handle::from_promise(p)) {}

    coro_handle coro_;
};


// ���[�U��`�R���[�`��
generator iota(int end)
{
    // �R���[�`���ɑΉ�����Promise�^ generator::promise_type��
    // Promise�I�u�W�F�N�g(p)�����������B

    for (int n = 0; n < end; ++n) {
        // ������ co_await p.yield_value(n) �Ɠ���
        co_yield n;
    }
    // �R���[�`���{�̂̏I�[���B�ɂ�� p.return_void() �Ăяo��
}


int main()
{
    // �R���[�`�����Ăяo���A���������W�F�l���[�^���擾����B
    auto g = iota(10);
    // ���̃^�C�~���O�ł͂܂��R���[�`���{�͎̂��s����Ȃ��B

    // �͈�for�\����p���ăR���[�`���{�̂����s����B
    // �����ł̓R���[�`��iota�̒l�������[�v�����X�e�b�v�ƁA
    // main�֐��̕\�����[�v�����X�e�b�v�����݂Ɏ��s�����B
    for (int v : g) {
        std::cout << v;
    }
};


